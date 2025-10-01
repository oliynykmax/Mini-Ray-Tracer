# miniRT: 2000 lines of code can make something cool

## 📸 Gallery

![Moon sphere with reflections](figure1.png)  
*A textured moon sphere with bump mapping and soft lighting. (Scene: moon.rt)*

![Sea](figure2.png)  
*Sea texture demonstration (sea.rt)*

![Desert landscape](figure3.png)  
*Paraboloids, boxes, and spheres in a sandy environment with textures. (Scene: desert.rt)*

![Reflections](figure4.png)  

## 🛠️ Technical Details

### Core Technologies
- **Language**: Pure C, Norm-compliant, no leaks, full error handling.
- **Graphics**: MLX42, minimal graphic library from codam.
- **Math Libraries**: Custom vector algebra, quaternion operations, and geometric utilities.
- **Concurrency**: POSIX threads to make rendering fly on multi-core CPUs.

### Advanced Features
- **Quaternion Rotations**: Smooth transformations using custom quaternion math.
- **Texture Mapping**: Bilinear-filtered PNG support with normal mapping for surface details.
- **Anti-Aliasing & DoF**: Random sampling for camera effects and noise reduction.
- **Real-Time Editing**: Mouse-based object manipulation with intuitive controls.
- **Multi-Threading**: Scalable parallel processing with job queues and synchronization.

## 🎮 Controls

- **Movement**: W/A/S/D to move forward/left/back/right, Space/Shift for up/down.
- **Object Selection**: Left-click on an object to select it.
- **Editing Modes**: Hold Z + click to translate, X + click to rotate, C + click to scale.
- **Quit**: ESC to exit.
- **Dump Scene**: F5 to save the current scene to a file.

## 📄 Scene File Format

Scenes are defined in .rt files with the following elements:

- **Ambient Light**: `A ratio R,G,B [secondary_R,G,B] [texture]`
- **Camera**: `C pos_x,y,z dir_x,y,z fov [focus_depth] [aperture]`
- **Light**: `L pos_x,y,z brightness R,G,B [radius]`
- **Sphere**: `sp pos_x,y,z radius R,G,B [rough] [metallic] [texture] [bump]`
- **Plane**: `pl pos_x,y,z normal_x,y,z R,G,B [rough] [metallic] [texture] [bump]`
- **Cylinder**: `cy pos_x,y,z normal_x,y,z radius height R,G,B [rough] [metallic] [texture] [bump]`
- **Paraboloid**: `pa pos_x,y,z normal_x,y,z radius height R,G,B [rough] [metallic] [texture] [bump]`
- **Box**: `bx pos_x,y,z normal_x,y,z size_x,y,z R,G,B [rough] [metallic] [texture] [bump]`

All values are floats, colors in 0-255 range, positions/normals in world coordinates.

## 📦 Getting Started

Ready to try it?

1. **Build it**:
   ```bash
   make
   ```

2. **Run a scene**:
   ```bash
   ./miniRT maps/std.rt
   ```

3. **Play around**: Load different .rt files from `maps/` and edit scenes live!

## 👥 Team Responsibilities

- **[oliynykmax](https://github.com/oliynykmax)**: Handled all the parsing, validation, and file stuff.
- **[datagore](https://github.com/datagore)**: Focused on the rendering core, math libs, and making it fast.
