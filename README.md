<p align="center">
    <img src="https://user-images.githubusercontent.com/12881812/210671438-4d622459-fc1b-4164-ba8a-932dd2926524.png" />
    <br>
    <a href="https://discord.gg/SDP4R6Wsza">Discord</a> • <a href="https://github.com/xezno/mocha/issues">Issues</a>
</p>

Mocha is a work-in-progress game engine that aims to (eventually) leverage modern graphics technologies to deliver high-quality visuals. It is equipped with a Vulkan renderer, a C# scripting system, and a hot-loading HTML/CSS user interface solution. In addition, Mocha offers a Jolt physics implementation and a physically-based shading and material system. The engine includes an ImGui-based editor for easy customization.

![Mocha Screenshot](https://user-images.githubusercontent.com/12881812/210655312-1e0d25a1-e4bf-49d9-943b-f88f932f7e08.png)

## Key Features

- High-quality visuals using a modern Vulkan 1.3 renderer with support for ray tracing (`VK_KHR_ray_query`)
- Flexible C# scripting with automatic C++ <-> C# interop bindings
- Customizable user interface using hot-loading HTML/CSS
- Realistic physics simulation with Jolt
- Work-in-progress physically-based shading and material implementation (currently without IBL)
- ImGui-based editor for easy customization and customization

## Building the Engine

### Prerequisites

- .NET 7.0
- vcpkg
- Visual Studio 2022
- Windows
- Vulkan SDK

### Instructions

1. First, clone the repository recursively with `git clone --recursive https://github.com/xezno/mocha`
2. Run **Source/setup_x64.cmd** if building for 64-bit (recommended), or **Source/setup_x86.cmd** if building for 32-bit.
3. Once complete, open **Source/Mocha.sln** and build the solution.
4. Run **Host** to start the engine and run SpaceGame.

## Contributing

Contributions to this project are greatly appreciated; please follow these steps in order to submit your contribution to the project:

1. Fork the project
2. Create a branch under the name `YourName/FeatureName`
3. Once you've made all the changes you need to make, go ahead and submit a Pull Request.

Throughout this project, the standard [C# coding conventions](https://docs.microsoft.com/en-us/dotnet/csharp/programming-guide/inside-a-program/coding-conventions) should be observed and used within any pull requests / any other direct contributions.

## Issues and Support

Feel free to [open an issue](https://github.com/xezno/mocha/issues/new) if you encounter any bugs or problems, have any feature requests, or have any questions.

## Free and Open Source

Mocha is an open source game engine distributed under the AGPL-3.0 license. This allows anyone to freely use, modify, and distribute the engine as they see fit. The development of Mocha is driven by the community, with the goal of creating a powerful and flexible tool that meets the needs of developers.

### License

```
Mocha Game Engine
Copyright (C) 2023  Alex Guthrie

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
```

A copy of this license is available at [LICENSE.md](https://github.com/xezno/mocha/blob/master/LICENSE.md).

## Acknowledgements
* [Badges](https://shields.io)
* [Icons](https://www.flaticon.com/)
