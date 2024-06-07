3D renderer written from scratch in C. **Purely** for educational purposes, as I want to better understand the
beautiful mathematics behind it all.

![example](./assets/demo.png)

---

#### Compilation

Download required submodule

-   `git submodule init`
-   `git submodule update`

And ensure `cmake` is installed.

#### Mac / Linux

Ensure sdl2, sdl2_image and sdl_ttf are installed on the system, then:

-   `cmake .`
-   `make`
-   `./bin/interplay`

#### Windows

Either setup SDL yourself or use the scripts located in `./misc` folder:

Do note, the scripts requires [ninja](https://github.com/ninja-build/ninja/releases) and [llvm](https://releases.llvm.org/download.html).

1. `.\misc\generate.bat`
2. `.\misc\build.bat`
3. `.\misc\run.bat`

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
