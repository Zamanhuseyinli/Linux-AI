# Linux AI

<div style="display: flex; align-items: center; gap: 90px;">
  <!-- Logo -->
  <a href="https://github.com/Zamanhuseyinli/Linux-AI/releases" target="_blank" rel="noopener noreferrer">
    <img src="https://raw.githubusercontent.com/Zamanhuseyinli/Linux-AI/refs/heads/main/LINUX-AI-LOGO.png" alt="Linux Collaboration AI" width="100" />
  </a>

  <!-- Contributors Badge -->
  <a href="https://github.com/Zamanhuseyinli/Linux-AI/graphs/contributors" target="_blank" rel="noopener noreferrer" style="display: inline-flex; align-items: center;">
    <img src="https://img.shields.io/github/contributors/Zamanhuseyinli/Linux-AI?color=green&style=flat-square" alt="Contributors" />
  </a>

  <!-- Issues Badge -->
  <a href="https://github.com/Zamanhuseyinli/Linux-AI/issues" target="_blank" rel="noopener noreferrer" style="display: inline-flex; align-items: center;">
    <img src="https://img.shields.io/github/issues/Zamanhuseyinli/Linux-AI?color=red&style=flat-square" alt="Issues" />
  </a>
</div>

---

## Linux Artificial Intelligence and Automation Structure

This project is based on large language models and a system-level automation framework. It enables users to interact with the system through kernel modules, while the AI layer handles I/O, device, and system components as if it were part of a robotic intelligence structure.

Tasks that normally require manual effort are handled quickly and efficiently by the AI system, allowing smoother and more autonomous interactions with your Linux environment.

Currently, the project supports core components such as I/O management, CPU control, and PCIe (PCIO) operations. The source code is open for contributors.  
Support the project via PayPal:  
👉 [Donate on PayPal](https://www.paypal.com/donate/?hosted_button_id=NKPHAU3NY3GZU)

---

## Kernel-Integrated Layers and Modules

Due to device support limitations, this project does **not** integrate with closed-source solutions such as `nvidia-smi` or similar proprietary APIs. Documentation for a `proc-GPU` support structure will be provided for contributors interested in extending GPU management.

Supported hardware currently includes Intel, AMD, and other integrated GPU and CPU components.

---

> ⚠️ **Warning:**  
> This is an experimental prototype.  
> It is **highly recommended** to install and test it in a virtual machine first.  
> Unexpected behavior may occur on physical devices.

---

## DE-AI

DE-AI is an AI management system designed specifically for desktop environments, providing limited AI support tailored to desktop use cases. It differs from Linux Kernel-AI, which offers full kernel-level AI control.

---

## Differences Between Linux Kernel AI and DE-AI

- **Kernel-AI:** Allows AI to take full control at the kernel level, enabling deep system integration and automation.  
- **DE-AI:** Provides limited AI functionality within desktop environments, without full kernel integration.

Linux-AI provides two layers of AI control:

- **ADMIN Layer:** Grants full control to AI, carrying significant risk as permissions cannot be revoked. Rebooting is recommended after ADMIN-level operations.  
- **USER Layer:** Allows limited, permission-based AI automation, safer for daily use.

---

## Common Tools

- `Kernel-AI-cpud`: Manages CPU resources in the background; requires ADMIN for full access.  
- `Kernel-AI-gpud`: Controls GPU resource allocation and management; ADMIN recommended.  
- `Kernel-AI-gpm`: Permission management tool for AI access levels.  
- `Kernel-AI-start`: Starts AI services manually or via systemd; ADMIN recommended.

---

## Setup

Linux-AI is primarily a kernel-level AI control system but can be installed alongside userspace components like DE-AI.

To install:

```bash
git clone https://github.com/Zamanhuseyinli/Linux-AI
cd Linux-AI
```

Build options:

- Using Python interpreter:

```bash
python Makemanifest.py
```

- Using make:

```bash
make
```

- Using CMake:

```bash
mkdir build
cd build
cmake ..
make
```

Two build modes:

- Python-based interpreter version.  
- Low-level kernel-integrated version via the `Makemanifest` tool.

Specify user privilege level during build (e.g., `Kernel-AI ADMIN`).

---

{{Warning|Kernel-AI is under active development. Test in virtual machines. Maintainers are not responsible for misuse or illegal activities. Limited NVIDIA-SMI support. Report issues at [GitHub Issues](https://github.com/Zamanhuseyinli/Linux-AI/issues).}}

---

## Support Status

The project is under development; some features may be incomplete. Testing in virtual machines or compiling as USER is advised until stable.

---

## AUR

To install via AUR (currently only KDE Plasma DE-AI supported):

```bash
yay -S plasma-system-ai
```

For precompiled binaries:

```bash
yay -S plasma-system-ai-bin
```
