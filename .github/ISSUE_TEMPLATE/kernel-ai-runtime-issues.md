---
name: Kernel-AI runtime issues
about: Kernel AI runtime for used issues.
title: KERNEL-AI-BUG
labels: bug, help wanted
assignees: Zamanhuseyinli

---

# Problems Using the Kernel AI Runtime

**Kernel-AI** is currently under development, and you're currently using version **`0.1.0d`**.  
There may be issues with some kernel interaction control mechanisms, so please **consider the consequences when compiling.**

---

## ⚙️ Compilation Recommendations

### ✅ Recommended Mode
Use the following for standard user-safe compilation:

```bash
make -LFS -USERPERMISSION
```

### ⚠️ Full Kernel Control Mode
If you require **full control** over the kernel, compile using:

```bash
make -LFS -ADMINPERMISSION
```

> **Warning:**  
> Linux-AI developers are **not responsible for any hardware issues** encountered after compiling with **admin permissions**.

---

## 🛠️ Custom Permission Compilation

To compile with **specific permissions**, you can grant the necessary permissions through the interface:

```bash
make -LFS -ADMINPERMISSIONER
```
or
```bash
kbuild ADMINPERMISSION
```
