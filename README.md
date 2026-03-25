# CocoUI Framework

CocoUI is a professional, hardware-agnostic, zero-allocation C++ UI framework designed strictly for constrained microcontrollers (from Cortex-M4 to Cortex-M7). 

Built with modern C++14, CocoUI abandons the bloated and verbose MVP (Model-View-Presenter) architectures typical of enterprise GUI tools, favoring a fluent, data-driven, and highly optimized Scene Graph suitable for deeply embedded systems with constrained RAM and Flash.

## 🎯 The Vision: Why CocoUI?

The embedded GUI landscape is dominated by heavyweights, but they come with severe architectural trade-offs that CocoUI aims to solve:

* **Vs. TouchGFX:** We eliminate the "God-Model" and the strict, file-heavy MVP boilerplate. CocoUI uses a lightweight Event Bus (Pub/Sub) for true decoupling, allowing rapid iteration without recompiling the entire state machine for a single button tweak.
* **Vs. LVGL:** We replace C-style callback spaghetti and `void*` type-unsafety with strict, strongly-typed C++14. If a UI component is wired incorrectly, it fails at compile-time, not via a Hard Fault in production.
* **Vs. Qt for MCUs:** We require zero proprietary precompilers (no `moc`) and zero QML parsing overhead. The UI is built using a zero-cost C++ Fluent API that compilers can aggressively optimize.

## 🧠 Core Architecture & Features

1. **Zero Dynamic Allocation:** No `malloc` or `new`. Everything is statically allocated or resides on the stack to guarantee deterministic behavior, prevent heap fragmentation, and pass strict industrial/medical coding standards.
2. **Professional Memory Alignment:** The core engine manipulates a purely mathematical `Framebuffer` using `alignas(32)`. This ensures seamless integration with hardware Direct Memory Access (DMA/DMA2D) and prevents L1 D-Cache coherence bugs on Cortex-M7 architectures.
3. **Hardware Agnostic:** CocoUI only calculates pixels. It doesn't know if it's running on an STM32, an NXP i.MX RT, or a Windows PC. You bring the display driver; CocoUI handles the math.
4. **Fluent API (Declarative C++):** UI creation reads like a declarative language, chained elegantly in a single file without external XML/JSON parsers. 
   *(e.g., `button.at({10, 20}).with_color(Colors::Blue).on_click(callback);`)*

---

## 🛠️ Prerequisites

To build the project and the PC simulator, you need the following tools installed in your system:
* **CMake** (v4.2 or higher)
* **LLVM / Clang** (v19 or higher)
* **Ninja Build System**

*Note: The PC Simulator utilizes [Raylib](https://www.raylib.com/) for hardware acceleration, which is fetched automatically by CMake during configuration.*

---

## 💻 Quick Start & Development Workflow

CocoUI uses `CMakePresets.json` to simplify the build process. Open your terminal in the root of the repository and follow these steps:

### 1. Configure the Project
Run this command first after cloning the repository or whenever you add new files. This generates the `build/` directory and configures the LLVM toolchain.
```powershell
cmake --preset windows-clang
```

### 2. Format the code.
To maintain a strict, professional standard, run the formatter before compiling. This automatically aligns brackets, pointers, and indentation using clang-format based on the Google Style Guide.
```powershell
cmake --build --preset format-code
```

### 3. Build the simulator
If the build reaches [100%] Built target cocoui_simulator, execute the generated binary to see the hardware abstraction layer in action:
```powershell
.\build\cocoui_simulator.exe
```
