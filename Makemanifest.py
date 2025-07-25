#!/usr/bin/env python3
import os
import sys
import platform
import subprocess
import shutil
import curses
import mimetypes
import xml.etree.ElementTree as ET

try:
    import yaml
except ImportError:
    yaml = None
try:
    import toml
except ImportError:
    toml = None


class MakeManifest:
    def __init__(self, filepath):
        self.filepath = filepath
        self.metadata = {}
        self.scripts = {}
        self.dependencies = []
        self.dev_dependencies = []
        self.user_define = {}
        self.defines = {}
        self.theme = None
        self.choice_prompt = None
        self.choice_index = None
        self.make_commands = []
        self.include_present = False
        self.raw_data = None

    def load(self):
        filename = os.path.basename(self.filepath)
        name, ext = os.path.splitext(filename)
        if name.lower() != "makemanifest" or ext != "":
            print(f"[ERROR] Unsupported manifest filename (must be exactly 'Makemanifest' without extension)")
            sys.exit(1)

        mime_type, _ = mimetypes.guess_type(self.filepath)

        if mime_type in ['application/xml', 'text/xml'] or self._is_xml_file():
            self._load_xml()
        elif mime_type in ['text/x-yaml', 'application/x-yaml'] or (yaml is not None and self._is_yaml_file()):
            self._load_yaml()
        elif toml is not None and self._is_toml_file():
            self._load_toml()
        else:
            try:
                self._load_xml()
            except Exception:
                print(f"[ERROR] Unsupported or invalid manifest content format.")
                sys.exit(1)

    def _is_xml_file(self):
        try:
            with open(self.filepath, 'r', encoding='utf-8') as f:
                start = f.read(100).lstrip()
                return start.startswith('<')
        except Exception:
            return False

    def _is_yaml_file(self):
        try:
            with open(self.filepath, 'r', encoding='utf-8') as f:
                text = f.read(500)
                return ':' in text and ('-' in text or '\n' in text)
        except Exception:
            return False

    def _is_toml_file(self):
        try:
            with open(self.filepath, 'r', encoding='utf-8') as f:
                text = f.read(500)
                return '=' in text and ('[' in text and ']' in text)
        except Exception:
            return False

    def _load_yaml(self):
        if yaml is None:
            print("[ERROR] PyYAML module not installed, cannot parse YAML manifest.")
            sys.exit(1)
        try:
            with open(self.filepath, 'r') as f:
                self.raw_data = yaml.safe_load(f)
            self._parse_dict(self.raw_data)
        except Exception as e:
            print(f"[ERROR] Failed to parse YAML manifest: {e}")
            sys.exit(1)

    def _load_toml(self):
        if toml is None:
            print("[ERROR] toml module not installed, cannot parse TOML manifest.")
            sys.exit(1)
        try:
            with open(self.filepath, 'r') as f:
                self.raw_data = toml.load(f)
            self._parse_dict(self.raw_data)
        except Exception as e:
            print(f"[ERROR] Failed to parse TOML manifest: {e}")
            sys.exit(1)

    def _load_xml(self):
        try:
            tree = ET.parse(self.filepath)
            self.root = tree.getroot()
            self.include_present = self.root.find('Include') is not None
            self._parse_xml_elements()
        except Exception as e:
            print(f"[ERROR] Failed to parse XML manifest: {e}")
            sys.exit(1)

    def _parse_xml_elements(self):
        meta = self.root.find('Metadata')
        if meta is not None:
            for tag in ['Name', 'Version', 'Description', 'Main']:
                el = meta.find(tag)
                self.metadata[tag.lower()] = el.text.strip() if el is not None else None

        scripts = self.root.find('Scripts')
        if scripts is not None:
            build = scripts.find('Build')
            clean = scripts.find('Clean')
            self.scripts['build'] = build.text.strip() if build is not None else None
            self.scripts['clean'] = clean.text.strip() if clean is not None else None

        deps = self.root.find('Dependencies')
        if deps is not None:
            for dep in deps.findall('Dependency'):
                name = dep.attrib.get('name')
                version = dep.attrib.get('version')
                self.dependencies.append((name, version))

        dev_deps = self.root.find('DevDependencies')
        if dev_deps is not None:
            for dep in dev_deps.findall('DevDependency'):
                name = dep.attrib.get('name')
                version = dep.attrib.get('version')
                self.dev_dependencies.append((name, version))

        ud = self.root.find('UserDefine/ByteDataSection')
        if ud is not None:
            self.user_define['size'] = int(ud.attrib.get('size', '0'))
            self.user_define['floatboat_encc'] = ud.attrib.get('floatboat_encc', 'no').lower() == 'yes'

        defines = self.root.find('Defines')
        if defines is not None:
            for d in defines.findall('Define'):
                name = d.attrib.get('name')
                value = d.attrib.get('value')
                self.defines[name] = value

        theme = self.root.find('Theme')
        if theme is not None:
            self.theme = theme.text.strip()

        choice = self.root.find('Choice')
        if choice is not None:
            self.choice_prompt = choice.attrib.get('prompt', None)
            self.choice_index = choice.attrib.get('choice_index', None)

        commands = []
        for mc_block in self.root.findall('MakeCommands'):
            for make in mc_block.findall('Make'):
                cmd = ''.join(make.itertext()).strip().replace('\n', ' ').replace('\r', ' ')
                cmd = ' '.join(cmd.split())
                commands.append(cmd)
        self.make_commands = commands

    def _parse_dict(self, data):
        def get_val(d, key):
            for k in d:
                if k.lower() == key.lower():
                    return d[k]
            return None

        metadata = get_val(data, 'Metadata') or get_val(data, 'metadata') or data
        self.metadata['name'] = get_val(metadata, 'Name') or get_val(metadata, 'name')
        self.metadata['version'] = get_val(metadata, 'Version') or get_val(metadata, 'version')
        self.metadata['description'] = get_val(metadata, 'Description') or get_val(metadata, 'description')
        self.metadata['main'] = get_val(metadata, 'Main') or get_val(metadata, 'main')

        scripts = get_val(data, 'Scripts') or get_val(data, 'scripts') or {}
        self.scripts['build'] = get_val(scripts, 'Build') or get_val(scripts, 'build')
        self.scripts['clean'] = get_val(scripts, 'Clean') or get_val(scripts, 'clean')

        self.dependencies = []
        deps = get_val(data, 'Dependencies') or get_val(data, 'dependencies') or {}
        if isinstance(deps, dict):
            for k, v in deps.items():
                self.dependencies.append((k, v))
        elif isinstance(deps, list):
            for item in deps:
                if isinstance(item, dict):
                    for k, v in item.items():
                        self.dependencies.append((k, v))
                else:
                    self.dependencies.append((item, None))

        self.dev_dependencies = []
        dev_deps = get_val(data, 'DevDependencies') or get_val(data, 'devdependencies') or get_val(data, 'devDependencies') or {}
        if isinstance(dev_deps, dict):
            for k, v in dev_deps.items():
                self.dev_dependencies.append((k, v))
        elif isinstance(dev_deps, list):
            for item in dev_deps:
                if isinstance(item, dict):
                    for k, v in item.items():
                        self.dev_dependencies.append((k, v))
                else:
                    self.dev_dependencies.append((item, None))

        user_define = get_val(data, 'UserDefine') or get_val(data, 'userdefine') or {}
        bds = None
        if isinstance(user_define, dict):
            bds = get_val(user_define, 'ByteDataSection') or get_val(user_define, 'bytedatasection')
        if isinstance(bds, dict):
            self.user_define['size'] = int(bds.get('size', 0))
            self.user_define['floatboat_encc'] = str(bds.get('floatboat_encc', 'no')).lower() == 'yes'

        self.defines = {}
        defines = get_val(data, 'Defines') or get_val(data, 'defines') or {}
        if isinstance(defines, dict):
            for k, v in defines.items():
                self.defines[k] = v
        elif isinstance(defines, list):
            for item in defines:
                if isinstance(item, dict):
                    for k, v in item.items():
                        self.defines[k] = v

        self.theme = get_val(data, 'Theme') or get_val(data, 'theme')

        self.make_commands = []
        mcs = get_val(data, 'MakeCommands') or get_val(data, 'makecommands') or []
        if isinstance(mcs, list):
            for mc in mcs:
                make_items = mc.get('Make') if isinstance(mc, dict) else None
                if isinstance(make_items, list):
                    for cmd in make_items:
                        if isinstance(cmd, str):
                            self.make_commands.append(cmd.strip())
                elif isinstance(make_items, str):
                    self.make_commands.append(make_items.strip())
                elif isinstance(mc, str):
                    self.make_commands.append(mc.strip())

    def run_make_commands(self):
        print("\nExecuting make commands:")
        for i, cmd in enumerate(self.make_commands, 1):
            for name, value in self.defines.items():
                cmd = cmd.replace(f"$({name})", value)
            print(f"\n[Executing command #{i}] {cmd}")
            result = subprocess.run(cmd, shell=True)
            if result.returncode != 0:
                print(f"[ERROR] Command #{i} failed with exit code {result.returncode}")
                sys.exit(result.returncode)
    def print_summary(self):
     # ANSI kaçış dizileri (Escape sequences)
     RESET = "\033[0m"
     BLUE = "\033[34m"
     GREEN = "\033[32m"
     RED = "\033[31m"
     YELLOW = "\033[33m"

     def print_line(label, value=None, color_label=BLUE, color_value=GREEN):
         if value is None:
             print(f"{color_label}{label}{RESET}")
         else:
             print(f"{color_label}{label}{RESET} {color_value}{value}{RESET}")

     print_line("Manifest Name:", self.metadata.get('name', 'N/A'))
     print_line("Version:", self.metadata.get('version', 'N/A'))
     print_line("Description:", self.metadata.get('description', 'N/A'))
     print_line("Main Executable:", self.metadata.get('main', 'N/A'))
     print_line("Theme:", self.theme if self.theme else 'None')
     print_line("Choice prompt:", self.choice_prompt if self.choice_prompt else 'None')
     print_line("Choice index:", self.choice_index if self.choice_index else 'None')
     print()

     print_line("Build script:", self.scripts.get('build', 'None'))
     print_line("Clean script:", self.scripts.get('clean', 'None'))
     print()

     print_line("UserDefine ByteDataSection size:", self.user_define.get('size', 'None'))
     print_line("UserDefine floatboat_encc:", self.user_define.get('floatboat_encc', 'None'))
     print()

     print_line("Defines:", color_label=YELLOW)
     if self.defines:
         for k, v in self.defines.items():
             print_line(f"  {k} =", color_label=YELLOW, color_value=GREEN)
             print(f"    {v}")
     else:
         print("  None")
     print()

     print_line("Dependencies:", color_label=YELLOW)
     if self.dependencies:
         for name, ver in self.dependencies:
             ver_str = ver if ver else ""
             print(f"  {name} {ver_str}")
     else:
         print("  None")
     print()

     print_line("DevDependencies:", color_label=YELLOW)
     if self.dev_dependencies:
         for name, ver in self.dev_dependencies:
             ver_str = ver if ver else ""
             print(f"  {name} {ver_str}")
     else:
         print("  None")
     print()

     print_line("Make commands:", color_label=YELLOW)
     if self.make_commands:
       for i, cmd in enumerate(self.make_commands, 1):
             print(f"  [{i}] {cmd}")
     else:
         print("  None")
     print()


    def prompt_choice(self):
        if self.choice_prompt:
            prompt = self.choice_prompt
        else:
            prompt = "Please enter your choice"

        if self.choice_index:
            allowed_separators = ['/', '|', '\\']
            sep = next((s for s in allowed_separators if s in self.choice_index), None)

            if not sep:
                print(f"[ERROR] Invalid choice_index: No valid separator found.")
                return False

            left, right = self.choice_index.split(sep, 1)
            left = left.strip()
            right = right.strip()

            print(f"{prompt} {left}{sep}{right}")

            while True:
                user_input = input().strip()
                if user_input == left:
                    return True
                elif user_input == right:
                    return False
                else:
                    print(f"[ERROR] Invalid input. Please type exactly '{left}' or '{right}'.")

        else:
            print(f"{prompt} 1/2")
            while True:
                user_input = input().strip()
                if user_input == '1':
                    return True
                elif user_input == '2':
                    return False
                else:
                    print("[ERROR] Invalid input. Please type '1' or '2'.")


def detect_linux_pkg_manager():
    if shutil.which('apt-get'):
        return 'apt'
    elif shutil.which('dnf'):
        return 'dnf'
    elif shutil.which('yum'):
        return 'yum'
    elif shutil.which('pacman'):
        return 'pacman'
    elif shutil.which('zypper'):
        return 'zypper'
    else:
        return None


def install_dependencies(dependencies):
    system = platform.system()
    pkgs = [dep[0] for dep in dependencies if dep[0] is not None]
    if not pkgs:
        print("[INFO] No dependencies to install.")
        return

    if system == 'Linux':
        pkg_manager = detect_linux_pkg_manager()
        if pkg_manager == 'apt':
            print("[INFO] Detected Debian/Ubuntu based system. Installing packages with apt-get.")
            subprocess.run(['sudo', 'apt-get', 'update'])
            subprocess.run(['sudo', 'apt-get', 'install', '-y'] + pkgs)
        elif pkg_manager == 'dnf':
            print("[INFO] Detected Fedora based system. Installing packages with dnf.")
            subprocess.run(['sudo', 'dnf', 'install', '-y'] + pkgs)
        elif pkg_manager == 'yum':
            print("[INFO] Detected RHEL/CentOS based system. Installing packages with yum.")
            subprocess.run(['sudo', 'yum', 'install', '-y'] + pkgs)
        elif pkg_manager == 'pacman':
            print("[INFO] Detected Arch based system. Installing packages with pacman.")
            subprocess.run(['sudo', 'pacman', '-Syu', '--noconfirm'] + pkgs)
        elif pkg_manager == 'zypper':
            print("[INFO] Detected openSUSE based system. Installing packages with zypper.")
            subprocess.run(['sudo', 'zypper', 'install', '-y'] + pkgs)
        else:
            print("[WARN] Could not detect package manager. Please install dependencies manually:")
            print(", ".join(pkgs))
    elif system == 'Darwin':
        print("[INFO] Detected macOS. Please install dependencies manually via 'brew install'.")
        print("Required packages:", ", ".join(pkgs))
    elif system == 'Windows':
        print("[INFO] Detected Windows. Please install dependencies manually.")
        print("Required packages:", ", ".join(pkgs))
    else:
        print(f"[WARN] Unknown system: {system}. Please install dependencies manually.")
        print("Required packages:", ", ".join(pkgs))


def run_makemanifest_header():
    print("[INFO] <Include> tag found. Running 'makemanifest-header.py'...")
    try:
        subprocess.run(['python3', 'makemanifest-header.py'], check=True)
        print("[INFO] 'makemanifest-header.py' completed successfully.")
    except subprocess.CalledProcessError as e:
        print(f"[ERROR] 'makemanifest-header.py' failed with exit code {e.returncode}.")
        sys.exit(e.returncode)


def find_manifest_file_case_insensitive(directory):
    files = os.listdir(directory)
    files_lower = {f.lower(): f for f in files}
    for f_lower, f_actual in files_lower.items():
        if f_lower == "makemanifest":
            return os.path.join(directory, f_actual)
    return None


def main():
    if len(sys.argv) < 2:
        manifest_path = find_manifest_file_case_insensitive(os.getcwd())
        if manifest_path is None:
            print("[ERROR] No manifest file named 'Makemanifest' found in current directory.")
            sys.exit(1)
        else:
            print(f"[INFO] Found manifest file: {manifest_path}")
    else:
        manifest_path = sys.argv[1]
        filename = os.path.basename(manifest_path)
        if os.path.splitext(filename)[0].lower() != "makemanifest":
            print(f"[ERROR] Manifest filename must be exactly 'Makemanifest' without extension.")
            sys.exit(1)

    manifest = MakeManifest(manifest_path)
    manifest.load()
    manifest.print_summary()

    install_dependencies(manifest.dependencies)

    if manifest.include_present:
        run_makemanifest_header()

    user_choice = manifest.prompt_choice()
    print(f"User selected: {user_choice}")

    manifest.run_make_commands()


if __name__ == "__main__":
    main()

