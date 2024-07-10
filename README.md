# EBPF-based Memory Leak Detection

Welcome to the EBPF-based Memory Leak Detection project! This repository contains a tool designed to detect memory leaks using eBPF (Extended Berkeley Packet Filter). The tool leverages eBPF to trace memory allocations and deallocations in the kernel, providing insights into potential memory leaks.

## Table of Contents
- [Introduction](#introduction)
- [Features](#features)
- [Requirements](#requirements)
- [Installation](#installation)
- [Usage](#usage)
- [Files and Directories](#files-and-directories)
- [Contributing](#contributing)
- [License](#license)

## Introduction
Memory leaks are a common issue in software development, often leading to degraded performance and system crashes. This project aims to provide a robust solution for detecting memory leaks in real-time by utilizing eBPF, a powerful tracing technology in the Linux kernel.

## Features
- Real-time detection of memory leaks
- Lightweight and efficient tracing using eBPF
- Comprehensive reporting of memory allocation and deallocation events
- Supports various types of memory allocation (kmalloc, vmalloc, etc.)

## Requirements
- Linux kernel version 4.9 or higher
- BPF and BPF Type Format (BTF) enabled in the kernel
- Clang/LLVM for compiling eBPF programs
- bpftool for loading and managing eBPF programs

## Installation
1. Clone the repository:
   ```sh
   git clone https://github.com/PrathumP/Ebpf_based_Memleak_detection.git
   cd Ebpf_based_Memleak_detection

2. Build the project:

   ```sh
   make

## Usage
1. Load the ebpf program :
   
   ```sh
   sudo ./memleak
  Monitor the output for detected memory leaks. The tool will display information about memory allocation and deallocation events, helping you identify potential leaks.

## Files and Directories

- README.md: This file, providing an overview and instructions for the project.
- Makefile: Build script for compiling the eBPF programs and user-space components.
- memleak.c: User-space component of the memory leak detection tool.
- memleak.bpf.c: eBPF program for tracing memory allocation and deallocation.
- memleak.h: Header file containing definitions and structures used in the project.
- trace_helpers.c: Helper functions for managing eBPF maps and programs.
- trace_helpers.h: Header file for the helper functions.
- maps.bpf.h: Definitions of eBPF maps used for storing tracing data.
- core_fixes.bpf.h: Workarounds and fixes for core BPF issues.

## Contributing 

Contributions are welcome! If you find any issues or have suggestions for improvements, please open an issue or submit a pull request. 

## License 

This project is licensed under the MIT License.
