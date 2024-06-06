3D renderer written from scratch in C. **Purely** for educational purposes, as I want to better understand the
beautiful mathematics behind it all.

![example](./assets/demo.png)

---

#### Compilation

Regardless of platform, run these two commands:

`git submodule init`

`git submodule update`

###### Linux

-   Ensure you have `SDL2`, `SDL2_image` and `SDL2_ttf` available on your system.
-   Run build script: `./build-linux.sh`

###### MacOS

-   Ensure you have `SDL2`, `SDL2_image` and `SDL2_ttf` extracted inside `/Library/Frameworks` folder.
-   `.dmg` files can be downloaded [here](https://github.com/libsdl-org/SDL/releases/download/release-2.30.0/SDL2-2.30.0.dmg), [here](https://github.com/libsdl-org/SDL_image/releases/download/release-2.8.2/SDL2_image-2.8.2.dmg) and [here](https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.22.0/SDL2_ttf-2.22.0.dmg), respectively.
-   Run build script: `./build-macos.sh`

###### Windows

-   Make sure you're in a developer console and have access to "CL.exe" compiler, x64 version.
-   Set SDL paths inside [build-windows-vc.bat](./build-windows-vc.bat) on line 15, 17 and 19.
-   Run the script i.e `.\build-windows-vc.bat`

---

#### Acknowledgements

I've used two primary resources, and I want to extend gratitude to the people behind those resources:

-   [Gustavo Pezzi](https://twitter.com/pikuma)

Gustavo's course [3D Computer Graphics Programming](https://pikuma.com/courses/learn-3d-computer-graphics-programming) has been my main resources to get an introductory understanding of 3D maths.
Gustavo is a phenomenal teacher with a great focus on mathematics. His clarity and thoroughness is something I value a great deal.
Besides, he also has a [YouTube channel](https://www.youtube.com/@pikuma) with loads of free awesome content.

-   [Fletcher Dunn](https://twitter.com/ZPostFacto)

The book [3D Math Primer for Graphics and Game Development](https://gamemath.com/book/intro.html)
contains excellent explanations (with problems) and has been an enjoyable read. It's even free! Thank you, Fletcher Dunn.
