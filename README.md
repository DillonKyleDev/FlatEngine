# FlatEngine - 2D Game Engine

![engine](https://github.com/user-attachments/assets/885f13e4-e57b-4232-9c16-bdee07d63437)


## UPDATE

There is now a published release for FlatEngine2D!  Please see the "Releases" section on the right-hand side for a download link and read up on the ReadMe for details on how to use FlatEngine.

I've included a sample project inside FlatEngine2D called FlatSpace.  With it, hopefully you can get an idea of how to use FlatEngine.  But remember, FlatSpace is only one way to make a game using FlatEngine.  Get creative!  You are welcome to use any art assets I created that are in the release for anything you want but the audio is audio I purchased on the Unity asset store so that can't be used as far as I know, except to experiment with.

As a note, FlatSpace was made before the implementation of persistant GameObjects which enable you to have some GameObjects stick around even when changing scenes.  Creation of new persistant GameObjects is located in the Assets dropdown menu.  Persistant objects are tied to the Project, so make sure to save your Scenes AND your Projects often to keep your progress saved!

### Building and using FlatEngine2D from source

If you are using the Release build of FlatEngine2D found in Releases you can safely ignore this section.  There currently is not a stable release for building from source so it is not recommended to do so at this time.  But if you wish to proceed anyway, here's how to get setup:

1. Download the .zip and extract
2. Run the Premake batch file by double clicking it located at `Premake/Setup-Windows.bat` to generate the Visual Studio solution file.
3. Open the solution file that was created in the root directory. Right click and build each project in the solution, (`Core/FlatEngine-Core`, `FlatEngine-Editor`, and `FlatEngine-Runtime`) in Debug configuration for debugging, and when you're ready for release, do the same in the Release configuration.

You're now set up to use the engine and edit it's source code as you wish.  From here, building a final game project is the same as it is with the main Release.

When you are finished editing the source code, you need to build the Editor project and the Runtime project in Release mode again. If you've made changes to the Core library, the Runtime project needs to know about those changes.

Now you should be able to go into the build location and open the FlatEngine-Runtime.exe with all of your source code changes present in the build.</br>

### Disclaimer

This project is in active development.

This is a hobby project.
It will likely crash eventually so save often. THERE IS NO AUTOSAVE. However, when loading into a new scene, or when starting the game loop, FlatEngine2D saves a temporary copy of your scene in `engine/tempFiles`. The engine will then load this copy once you stop the game loop, preserving changes made after last save but before starting the game loop.
Things will change with updates.
Your results and the usefulness of FlatEngine may vary.
Some key systems are not optimized.
The Box2D implementation is only partly complete and the components section has yet to be updated to reflect the changes. Expect ReadMe documentation changes to accompany the next major release of FlatEngine2D.

--------------------------------------------------------------------------------------
Quick links:
1. <a href="https://github.com/DillonKyleDev/FlatEngine2D?tab=readme-ov-file#viewports">Viewports</a>
1. <a href="https://github.com/DillonKyleDev/FlatEngine2D?tab=readme-ov-file#engine-components">Engine Components</a>
2. <a href="https://github.com/DillonKyleDev/FlatEngine2D?tab=readme-ov-file#using-flatengine">Quick Start Guide</a>
3. <a href="https://github.com/DillonKyleDev/FlatEngine2D?tab=readme-ov-file#building-the-final-project">Building a completed Project</a>
4. <a href="https://github.com/DillonKyleDev/FlatEngine2D?tab=readme-ov-file#lua-scripting-in-flatengine">Lua Scripting in FlatEngine2D</a>
5. <a href="https://github.com/DillonKyleDev/FlatEngine2D?tab=readme-ov-file#lua-functions-available">Lua-FlatEngine2D scripting functions</a>
6. <a href="https://github.com/DillonKyleDev/FlatEngine2D?tab=readme-ov-file#flatengine-classes-exposed-to-lua---lua-usertypes">FlatEngine2D classes exposed to Lua (Lua usertypes)</a>
--------------------------------------------------------------------------------------

## About:

FlatEngine2D is a 2D game engine made using SDL2, ImGui, and Box2D.  It is a passion project and is intended to replace Unity in all of my future 2D game development projects.  Maybe you will find it useful as well.  I will also be using the bones of the engine I develop here to create a full 3D game engine in the future.  See the license for details about using the engine and code, but as far as code I have written and assets I've created, you are free to use them as you wish.  Additionally, I would love to see any projects you are working on or have used any of the engine code for if you would be willing to share it.

I've spent a lot of time with the Unity game engine and so have found comfort in it's design philosophy.  I will be adopting many of Unity's methods for handling various things within my engine.


--------------------------------------------------------------------------------------


## Libraries / Utilities
| Library used        | Features supported |
|:--------------------|:---------------|
|SDL_2|The engine is built using SDL_2 as a base, along with several other SDL libraries including SDL_ttf for fonts, SDL_Image, and SDL_Mixer for audio.|
|ImGui|The entire user interface, including all of the interactions within the scene view, are handled using ImGui_Docking.|
|Box2D|The physics and collision in FlatEngine2D is handled by Box2D.|
|nlohmann Json Formatter|nlohmann Json Formatter is used for saving various types of data for later use including Scene data (GameObjects and components), Project data, and Animation data.|
|Lua/Sol|Lua/Sol is the scripting language embedded in FlatEngine2D. Scripts are contained in files with the extension ".scp.lua" in order to be controlled as needed within the engine.  See "Using FlatEngine" below for a detailed walkthrough of how to use Lua in FlatEngine.|


--------------------------------------------------------------------------------------

## Viewports

All available viewports can be seen in the Viewports dropdown menu on the main top bar menu.  Here you can toggle each one visible or invisible.  You can freely drag each viewport to whatever location in the engine you want and can dock them inside each other.  Just grab a viewport by the top tab bar and put it where you want it.
____________________________________________________________________________________________________________
![viewports](https://github.com/user-attachments/assets/b1bf8081-cfd3-4ee3-8903-f43aae22ad2f)

----------------------------------------------------------------------------------------

### Scene View

The Scene View is the viewport where you'll be spending most of your time.  Here you will layout your GameObjects and set up your Scenes and are able to see the active boundaries of GameObject components like Canvases, Cameras, BoxColliders and Buttons.
____________________________________________________________________________________________________________
![sceneView](https://github.com/user-attachments/assets/01fc138e-8675-4182-b6fc-154918711973)

### Game View

The Game View displays the actual final look your game will have when it is built and running.  There are no widgets displayed here, only the final result of all your hard work.
____________________________________________________________________________________________________________
![gameView](https://github.com/user-attachments/assets/279db1db-4a67-476e-8c7c-b52eb80ca0a3)

### Inspector

The Inspector viewport is where you will edit all of the values of each GameObject's components.  Click on a GameObject either in the Scene Hierarchy, Persistant GameObject Hierarchy, or within the Scene View to focus it in the Inspector window.  Once focused, each component attached to the object will be shown and editable and you are able to rename the GameObject.  Each component can be collapsed to conserve space within the view and you can even collapse them all at the same time by clicking the button to the left of the "Tags" button.
____________________________________________________________________________________________________________
![inspector](https://github.com/user-attachments/assets/173fc721-7a84-4edd-85a5-6b805c9a9a74)
____________________________________________________________________________________________________________
The final and perhaps most important feature of the Inspector window is the ability to add new components to GameObjects.  You can do this either by clicking the three dots expand button on the top right or using the big purple "Add Component" button on the very bottom of the viewport.
____________________________________________________________________________________________________________
![addComponent](https://github.com/user-attachments/assets/3cf147ac-54d5-41b9-a3fc-55e668d022bc)

### Hierarchy

The Hierarchies are where you will find the list of all GameObjects in your Scene and in your Project.  Individual Scenes each hold their own set of GameObjects.  In addition to those objects there are also a set of Persistant GameObjects that belong to each Project.  These objects don't get unloaded when you change Scenes and they are visible in the Persistant Hierarchy.  All regular Scene objects are found in the Hierarchy viewport.  These viewports function identically to one another and allow you to visually see the structure of your Scene.  Each GameObject can have many nested children that will appear in a collapsable section under each parent object.  You can click and drag GameObjects onto other GameObjects to set them as children of those objects.  Remove a GameObject as a child by dragging it into a space between two GameObjects in the Hierarchy.
____________________________________________________________________________________________________________
![hierarchies](https://github.com/user-attachments/assets/9cd84272-a9e5-4bb7-b74a-579032597c8a)


### Log

Where all text output and error messages are displayed.  Messages from the engine are prefixed `[C++]` and messages from Lua scripts you've written are prefixed `[LUA]`.  All logging functions available to you are in the Lua sections of this README toward the bottom.
____________________________________________________________________________________________________________
![logger](https://github.com/user-attachments/assets/74cf062b-8c8a-4417-861a-e5c2a64c840d)

### Animator & Keyframe Editor

The Animator lets you animate component properties of GameObjects.  You can select a component property to animate in the dropdown menu and add it to the Animation to cause it to pop up in the timeline window with a new keyframe to edit.  If you select the keyframe with the Keyframe Editor open like it is in the photo, you can see all of the properties you are able to edit on the selected keyframe.  If you want to animate a specific property, make sure the checkbox is checked next to it.  You can move the keyframe pip inside the Animation Timeline directly to change the time it is played by clicking and dragging it or you can directly input the keyframe time in the Keyframe Editor window.  Don't forget to save any changes made to the Animation using the three dots expand button in the main Animation viewport.
____________________________________________________________________________________________________________
![animatorKeyFrameEditor](https://github.com/user-attachments/assets/f996a862-6877-4a80-b406-43727f4ca49e)

### File Explorer

The File Explorer is a real-time representation of the folder structure in the current Project directory.  The currently loaded Project's asset directory will be displayed in the `Directories` hierarchy on the left-hand panel.  It is navigable either via the directory tree or by double clicking folders in the right-hand panel.</br>
____________________________________________________________________________________________________________
![fileExplorerLayout](https://github.com/user-attachments/assets/ed8de4d5-e7e6-4143-a5ac-896e506d7a06)</br>
____________________________________________________________________________________________________________
The File Explorer supports creation and deletion of assets through the right-click context menu. *Files deleted here are well and truly deleted from your computer*.</br>
  ____________________________________________________________________________________________________________
![rightClickContext](https://github.com/user-attachments/assets/be7a3472-92dc-473e-afde-d40923da1c49)</br>
____________________________________________________________________________________________________________
You can also drag and drop assets directly into the Scene View to create GameObjects from them like prefabs and images. You can also drag images, animation files, audio files, and font (.tff) files directly from the File Explorer into GameObject components to use them as references in Sprite, Text, Animation and Audio compnonents.</br>

### Project File Structure in FlatEngine2D
When a Project is created in FlatEngine2D, a new directory with the Project's name is created inside the `projects/` directory.  Each project folder is created with the same set of directories upon creation:</br>
1. animations/
2. audio/
3. images/
4. images/tileTextures/
5. mappingContexts/
6. prefabs/
7. scenes/
8. scripts/
9. tileSets/

When a game Project is finished and built, only the Project that is currently open in the editor will be copied over to the build location, so please only put assets you are using for the current Project in the directory with the Project's name. *Don't use assets in other Project folders.*
____________________________________________________________________________________________________________
### Mapping Context Editor

Using the Mapping Context Editor you can bind input actions to specific keys and buttons and save them to be accessed through Lua scripts.  You can either create new Mapping Contexts through the editor or through the right-click context menu in the File Explorer window.  Doing so will automatically open the editor with the new Mapping Context opened and ready to edit.  Don't forget to save all your changes after adding your input actions!
____________________________________________________________________________________________________________
![mappingContextEditor](https://github.com/user-attachments/assets/d45c57da-b8b8-4a23-bf1e-d6259d3f21d8)

### TileSet Editor

Note: This section is currently subject to significant rework as I add Box2D into FlatEngine2D.</br>
Using the TileSet Editor you can create the TileSets that you'll use inside the TileMap components.  All you have to do is create a new TileSet either in the TileSet Editor or in the File Explorer right-click context menu and drag and drop an image from the File Explorer into the input as your tiling image.  This component is still early in development so it only allows a 16x16 selection for each tile for now.  Once an image is chosen, click on each tile you want to be included in your TileSet and if your TileSet is selected in a TileMap component, you should see them start to appear as available tiles to paint with.
____________________________________________________________________________________________________________
![tileSetEditor](https://github.com/user-attachments/assets/2319b498-71ee-432c-92af-523a79acfdd6)
____________________________________________________________________________________________________________
Change your brush mode to Tile Drawing mode by clicking on the brush icon in the top left corner of the Scene View.  Your other options are:
1. Translate mode (default, for moving GameObjects around within the Scene View)
2. Tile Brush mode: For drawing selected tiles on a selected TileMap
3. Erase mode: For erasing tiles drawn
4. BoxCollider draw mode: This mode allows you to create collision areas for your TileMaps that line up with the tile grid nicely.  It's still a very young feature and is pretty unpolished, but it works in it's current state.
5. Tile Select mode: For selecting multiple tiles in order to move them to somewhere else within the TileMap using the Tile Move mode.
6. Tile Move mode: For moving selected groups of tiles within the TileMap.  This feature is also very unpolished and I don't recommend relying on it too much.
   
Once you're in Tile Brush mode, click on the tile you want to draw in the TileMap component and start drawing!

![brushModeSelect](https://github.com/user-attachments/assets/767f2555-066b-4cfd-ac20-94e0fb90d668)

If you want to try out the collision area drawing feature, great!  Scroll down to the bottom of the TileMap component to create a Collision Area.  Then either click "Draw Collision Areas" or choose the BoxCollision Draw Mode brush in the top right of the Scene View again:

![boxCollisionDrawMode](https://github.com/user-attachments/assets/2a9205b8-4bff-4a6d-8206-93fe12cfee61)

Click and drag to create as many BoxColliders as you wish:
____________________________________________________________________________________________________________
![collisionAreaDrawn](https://github.com/user-attachments/assets/bdaf0134-8856-4d88-8a29-e7f4fa009e1e)
____________________________________________________________________________________________________________
When you're done, back in the TileMap component click "Generate Colliders" and your Collision Areas will be combined into one big collider.  You can then give the attached GameObject any Tags (just like any other GameObject) and the Collision system will respect those Tags.  Remember, the TileMap system is still underdeveloped, so your experience with it will vary.</br>
____________________________________________________________________________________________________________
![collisionAreaGenerated](https://github.com/user-attachments/assets/12fe97a2-40d8-414c-b4d5-1ec1b96d0eca)


--------------------------------------------------------------------------------------


## Engine Assets

There are several key assets that you will be spending a lot of time with in FlatEngine2D.  Most of these can be created in several ways, but the main two are through the main menu top bar: `Create -> Asset files -> asset_type`, and through the right-click context menue in the File Explorer window.

-------------------------------------------------------------------------------------------
![createAssets1](https://github.com/user-attachments/assets/20b386f5-3647-481c-89af-15811e38a3ff) or 
![newAssets2](https://github.com/user-attachments/assets/85b10b5b-9f6c-4b77-9c7b-f4ed3a3ba8da)
-------------------------------------------------------------------------------------------
<img alt="Scenes" src="https://github.com/user-attachments/assets/7fa21df7-cfc4-490e-8d17-836775eb90b5"></br>
Projects contain all the data associated with a particular game-in-progress including the Scene to load up at the start of a game and the list of Persistant GameObjects in the game, (game managers, etc.), but mostly information that is not relevant to the end user. Project files are saved in JSON format.  New Projects are made in the Project Hub when you first launch the engine.</br>

<img alt="Scenes" src="https://github.com/user-attachments/assets/24bb752a-9bd9-45d1-937a-5692bfa1b1bc"></br>
Scenes are the container for all GameObjects and are saved in JSON format using a JSON Formatting library for simple parsing.</br>

<img alt="TileSet" src="https://github.com/user-attachments/assets/33c561a0-40b5-4d0d-ab76-2a4ff9b8d828"></br>
TileSets are used by the TileMap system to allow them to draw texture tiles onto TileMap components in the Scene View.</br>

<img alt="Script" src="https://github.com/user-attachments/assets/9c2e9d4b-ab18-4624-b7ab-b213d16e306a"></br>
Lua script files are for scripting.  In FlatEngine, these files have the extension ".scp.lua" in order to be seen and processed by the engine correctly. You create new Script files in the File Explorer window by right clicking and adding the new asset or inside a Script component in the Inspector window.</br>

![newScript](https://github.com/user-attachments/assets/6bba31c5-beb4-4ed7-a1a2-9fce5679b5b4)

<img alt="Mapping Context" src="https://github.com/user-attachments/assets/fc6ed04f-5fb0-476d-a076-c1c7c1b9d181"></br>
Mapping Contexts (and the Mapping Context Editor) are the interface for binding input actions to specific controls.  Currently mouse/keyboard and XInput (Xbox controller) are supported with more planned to be added in the future.  You can programmatically change bindings using Lua Scripts.</br>

<img alt="Animation" src="https://github.com/user-attachments/assets/e88f98cd-855e-40c8-8b32-e3759fcbdd08"></br>
Animations are what the Animation components use to play component Animations.  Animations are edited using the Animator window in combination with the Keyframe Editor and Animation Timeline.  If you have used Unity then you should be familiar with how this functions.</br>

<img alt="Prefab" src="https://github.com/user-attachments/assets/f0bb4978-2afe-425b-9ac0-6a09710e92ca"></br>

Prefabs are saved GameObjects (including any children and components) that can be instantiated at runtime, or at any time, into the Scene.  To create a new Prefab, right click a GameObject in either of the two Hierarchy windows and select `Create Prefab`.  GameObjects that are already Prefabs will show more options to select when right clicking.  You will be able to save changes made to this object and extend those changes to all other Prefabs of the same name, unpack the Prefab (Remove it from the Prefab system without changing any of it's properties so that any changes made to it can be saved without effecting the Prefab family), or create a brand new Prefab with a new name based on that GameObject.  Creating a new Prefab from an existing Prefab won't change any Prefabs that were made using the old Prefab name. You can drag and drop Prefabs from the File Explorer into the Scene or create them from the top menu bar in `Create -> Prefabs -> your_existing_prefab`.  You can also instantiate prefabs using Lua with `Instantiate(std::string prefabName, Vector2 position)` where the `position` is the (x,y) position to instantiate the Prefab in the Scene.

![createPrefabs](https://github.com/user-attachments/assets/ca76e4f5-5aad-46be-ab91-ea3a88ef5a12)

--------------------------------------------------------------------------------------


## Engine Components

### Tags

Tags are a list of arbitrary properties that a GameObject can have that can be queried using 

`GameObject::HasTag("tagName")` 

You can set a GameObject to have a specific tag using `GameObject::SetTag("tagName", true);` or, more conveniently, set it in the Inspector viewport by clicking the `Tags` button below the object's name:

--------------------------------------------------------------------------------------

![tagsbutton](https://github.com/user-attachments/assets/527d1366-83b2-4f64-b54e-90357daab056) <b>--->>>></b>
![availableTags](https://github.com/user-attachments/assets/e0030132-1724-416b-bf37-a8dc983eb9a3)

--------------------------------------------------------------------------------------

The Tags system is also used by the collision detection system to know what objects can collide with other objects, based on the Tags each GameObject has and is set to Collide with.  Tags are completely customizeable by opening and editing the `Tags.lua` file found in `/engine/scripts/Tags.lua`.  Here you can freely add and remove available Tags you can choose from for GameObjects and they will appear in the tag list in engine upon either closing and reopening the FlatEngine or by clicking "Reload Tags" in the dropdown menu under `Settings -> Reload Tags` on the main menu bar. This is also how you reload any customizations you've made to the color scheme of FlatEngine2D in the `Colors.lua` file or engine icons in `Textures.lua` located in the same directory as `Tags.lua`.  (Your changes will also go into effect upon relaunching the engine)</br></br>
![reloadTags](https://github.com/user-attachments/assets/203d64aa-9d41-4e50-8a6e-e62461c1d266)

--------------------------------------------------------------------------------------

Tags must remain in the format present in the `Tags.lua` file: Comma-separated strings inside a Lua table named `F_Tags`</br>

`F_Tags = {
  "Tagname",
  "Tagname2",
  "Tagname3"
}`</br>
or</br>
`F_Tags =` {</br>
  `"Tagname",`</br>
  `"Tagname2",`</br>
  `"Tagname3"`</br>
`}`</br>

## Components

FlatEngine2D's GameObjects require components be attached for their functionality.  The current list of components included in FlatEngine are:

1. Transform
2. Sprite
3. Camera
4. Script
5. Button
6. Canvas
7. Camera
8. Animation
9. Audio
10. Text
11. BoxBody
12. CircleBody
13. CapsuleBody
14. PolygonBody
15. ChainBody
16. CharacterController
17. TileMap (work in progress)

____________________________________________________________________________________________________________
### Transform

All GameObjects MUST have a Transform and are created with one that cannot be removed.  Transforms handle:

| Property        | Description |
|:--------------------|:---------------|
|Origin| The reference point for all position changes.|
|Position| An (x,y) Vector2 that holds position relative to the Origin point.|
|Scale| The Scale of the entire object and its components (seperate from any additional component scale parameters, ie.. Sprite scale). Children are not yet affected by the scale of their parent. I am still working out how I want this aspect of scaling to be handled.|
|Rotation| The rotation in degrees of the object.  Currently the collision system does not account for rotations of objects so Sprites are all that are affected by this.|
____________________________________________________________________________________________________________
### Sprite

Sprites are the visual representation of GameObjects in the Scenes.  Supported image formats for Sprites are:</br>
- .png
- .jpg
- .tif
- .webp
- .jxl

Sprites have the following properties:

| Property        | Description |
|:--------------------|:---------------|
|Scale| The scale of the image (separate from the scale of the Transform).|
|Offset| The position of the Sprite relative to the Transforms position, usually just (0,0).|
|Render Order| Determines what other images the image will be rendererd in front of or behind. The lower the render order, the farther back it will be rendered in the scene.|
|Tint color| The tint color that will be applied to the texture.|
|Pivot Point| The point at which all scaling and positioning of the texture is relative to.  If you manually change the Offset of the Sprite, that value will override the Pivot Point offset.|

### Script

Scripts in FlatEngine are written in Lua in ".scp.lua" files.  The Script component is a container for a script reference and allows you to expose variables to the lua script from the inspector window for customizeability across GameObjects using the same Script.  See the more detailed explanation further down on how to use the Script components and Lua in FlatEngine2D.

### Button

Buttons are simple UI elements for enabling mouse interaction in a scene.  Buttons are meant to be used in combination with Canvas components in order to block Buttons underneath them, but it is not strictly necessary.  Currently they are only natively supported with mouse controls, but it shouldn't be too difficult to enable gamepad support through Lua scripting.  Buttons have the following properties

| Property        | Description |
|:--------------------|:---------------|
|Active Layer| The layer that the Button is active on.  If another Button is on the layer above this one and you hover over it, the Button on the lower level will not be activatable.  Similarly, if you use a Canvas component and set the Button to be on the layer of or above the Canvas, the Canvas will completely block, (if enabled), all Button interactions below the Canvas layer.|
|Active Dimensions| The width and height of the button in grid spaces.|
|Active Offset| The position of the Button relative to the Transforms position.|

### Canvas

Canvases are meant to support the use of Buttons when making UIs.  Canvases, (if enabled), block all Button interactions of Buttons that are below the layer of the Canvas.  Canvases have the following properties

| Property        | Description |
|:--------------------|:---------------|
|Active Layer| As discussed, Buttons below this layer and within the bounds (width and height) of the Canvas will be blocked from interaction.|
|Dimensions| The width and height of the Canvas.|

### Camera

The Camera component is how the scene is viewed in the GameView.  Even without a Camera, the GameView is still viewable, but it will be locked to the center (0,0) of the world at a fixed zoom level.  You can have multiple Cameras per scene and switch between them freely using the b_isPrimaryCamera check.  Objects that are outside the viewing width and height of the primary scene Camera are not rendered.  Cameras have the following properties

| Property        | Description |
|:--------------------|:---------------|
|Dimensions| The with and height of the Camera, determines where the cutoff is for objects being rendered.|
|Is Primary Camera?| Determines which Camera the GameView will be rendered through.|
|Zoom| Determines how zoomed in the Camera is.|
|Follow Smoothing| Determines how quickly the camera snaps to its follow target.|
|Follow Target| The GameObject, if any, this Camera will follow, if enabled.|
|Should Follow?| Toggles whether the Camera should follow the Follow Target or stay where it is.|

### Animation

Animation components are one of the most powerful components that can be attached to a GameObject.  Animations are attached to Animation components of a GameObject and can be called through the Animation::Play() method at runtime.  Animations currently support animation of Transform, Sprite, Text, and Audio components, and Events. Animation Events allow you to call Lua functions at a particular keyframe and pass up to 5 parameters to it, (string, int, long, float, double, bool, or Vector2).  These are extremely powerful and useful to take advantage of.  I plan to add more components to the list of components you can animate, but I believe this is a very versitile grouping and likely you are able to do most of what you need to with them.  I will update this with more detailed information in the future.

### Audio

The Audio component allows you to attach several different audio clips and music files to a single GameObject and call them by name in Lua, or play them from the Animation component.  Each sound clip in an Audio component has the following properties:

| Property        | Description |
|:--------------------|:---------------|
|Name| The name used to play the Audio clip.|
|Filepath| The path of the audio file to be played.|
|Sounds| An unlimited number of sounds can be attached to each Audio component and can be accessed using their individual names.|


### Text

Text components function very similarly to Sprite components except they render text.  Text components have the following properties:

| Property        | Description |
|:--------------------|:---------------|
Text|The actual text to be rendered.|
|Font| The font that will be used|
|Scale| The scale of the text (separate from the scale of the Transform).|
|Offset| The position of the Text relative to the Transforms position, usually just (0,0).|
|Render Order| Determines what other images the image will be rendererd in front of or behind. The lower the render order, the farther back it will be rendered in the scene.|
|Color| The color that will be applied to the texture.|
|Pivot Point| The point at which all scaling and positioning of the texture is relative to.  If you manually change the Offset of the Text, that value will override the Pivot Point offset.|

### Collision Components

Detailed documentation on Box2D implementation coming soon.


### CharacterController

Note: This component is due for a rework.  The exposed variables don't make much sense and are not intuitive to use.  Max Acceleration is essentially the max speed, or the maximum amount of velocity that is added each frame.

The CharacterController is a bundle of functionallity that is meant to make it easier to manipulate a character GameObjects RigidBody component.  The CharacterController has the following properties:

| Property        | Description |
|:--------------------|:---------------|
|Max Speed|Determines the maximum speed a GameObject can move.|
|Max Acceleration|Determines how fast the GameObject gets to its Max Speed (not really, see note above)|
|Air Control|Determines how freely this GameObject can move while not grounded.|

### TileMap

Note: The TileMap component is in working condition but it does need some improvements to the user experience and optimization to be considered complete.  I am including it in the release for testing purposes.

The TileMap component allows the user to quickly draw scenes using TileSets created in the engine.  Each TileMap can have multiple TileSets (palettes) that it can use to draw in the scene.  Eventually I would like to add layers to each TileMap, but for now, in order to have layered tiles you must create another GameObject with a TileMap and place them on top of each other.  I recommend having one parent object and then as many child GameObjects as needed for the layers needed.  The TileMap currently supports a rudimentary BoxCollider drawing system that can be used to add collisions to the TileMap.  It is surely in need of a user experience overhaul but it is functional.  As noted above, there are several optimizations that still need to be made to this system to be considered complete.  The TileMap component contains the following properties:

| Property        | Description |
|:--------------------|:---------------|
| Dimensions      | The width and height of the TileMap drawing canvas.|
| Tile Dimensions | The dimensions of the actual texture tiles you are drawing with in pixels. (default 16px)|
| Render Order    | Just like with the Sprite and Text component, this determines what other textures will be drawn over and under the TileMap.|
| TileSets        | The list of available TileSets you can use to draw with for this TileMap.
| Collision Areas | Sets of colliders that the user can draw in the TileMap that function identically to the BoxCollider components (because they are under the hood).|

-----------------------------------------------------------------------------------------------------------------

## Using FlatEngine

WARNING: FlatEngine is NOT a complete engine and as such, it will almost certainly crash and cause you to lose unsaved progress so please save both your Scenes and your Project often.  There's a handy dropdown in both of the Hierarchies that lets you "Save all", which will save both the loaded Scene and the Project in one click.  If you do experience a crash, which you likely will eventually, if you are willing, please submit an issue in the repository with the details on how to recreate it.  That would be extremely helpful to me and I would greatly appreciate it!  If not, that's okay too.

With the warning out of the way... FlatEngine is intended to be very straightforward to use and very fast to get up and running on a project.  

In FlatEngine, the general flow is:

<h3>Project Creation</h3>
Create a project using the Project Hub


<h3>Scene Creation</h3>
Create a scene and open it.  You can do this in two ways:

1. `File > New Scene`
2. Open the File Explorer viewport:

`Viewports > File Explorer`

Then, in an empty space in the File Explorer, (preferably in the scenes folder), `Right click > Add New > Scene`  ... Then double click the new scene to open it.


<h3>Add GameObjects to the Scene</h3>
You can add GameObjects to the Scene using the top menu bar in several ways:

<br/>`Create > GameObject`<br/>
`Create > Components`  (allows for quick creation of GameObject with a default component)
`Create > Prefabs`     (once you have created a Prefab by right clicking on an existing GameObject, you can instantiate them from here)


<h3>Add Components to the GameObjects</h3>
If the Inspector window is open, (Viewports -> Inspector) you can click on a GameObject to focus on it and view its components in the Inspecor viewport.  Here you have two options to add components:

1. Three dots expand button under the GameObjects name then click "Add Component"
2. The purple Add Component button at the bottom of the Inspector viewport


<h3>Add Controls</h3>
Create a Mapping Context in one of two ways:

1. `Create > Asset files > Mapping Context`<br/>
2. In empty space in the File Explorer viewport, `Right click > Add new > Mapping Context`

After you have created a new Mapping Context, you can double click it in the File Explorer to open the Mapping Context Editor or open the editor using:

<br/>`Viewports > Mapping Context Editor`<br/>

Select the Mapping Context you'd like to edit and add an input action name along with the button you'd like to associate it with.  Click "Add" and don't forget to "Save" in the top right of the viewport.

In order to access these newly created bindings, you will need to add a Script component to the GameObject you want to have access to it.  In the Inspector window for the GameObject, Add a new Script component.  In the Script component click "New Script" and give it a name, then select the newly created script file in the Script components dropdown menu.  Now open up the Script in your favorite text editor and in `Awake()` (inside the data variable) add a mapping context variable and assign it to the mapping context you just created.  This can be done like so:

NOTE: Please see "Lua Scripting in FlatEngine" below for a more detailed explanation of scripting in FlatEngine.

-- Assume the Script name is "PlayerController" and the Mapping Context is named "MC_Player".

```
-- PlayerController.scp.lua

function Awake() 
    PlayerController[my_id] = 
    {
        mappingContext = GetMappingContext("MC_Player")
    }        
end 
```

You can then in a later function access the `mappingContext` variable to query the input action.  There are two ways to query an input action:

```
function Update()
    local data = PlayerController[my_id]

    if data.mappingContext:ActionPressed("IA_MoveForward") then
        -- do moving things
    end

    if data.mappingContext:Fired("IA_Jump") then
        -- do moving things
    end
end
```

`bool ActionPressed()` - returns true if the button is currently being pressed
`bool Fired()` - returns true only on the initial press of the button


<h3>Add Animations</h3>
Animations can be used to do nearly everything sequencial you need to accomplish in your game, you just need a little creativity.  You can create an Animation in two ways:

1. `Create > Asset files > Animation`
2. In the File Explorer in empty space `Right click > Add new > Animation`

Open the newly created Animation by double clicking the file in the File Explorer or by opening the Animator window (`Viewports > Animator`) along with the Keyframe Editor (`Viewports > Keyframe Editor`).

From here you can open your Animation file using the expand button menu in the Animator window. (If you double clicked it should be opened in the Animator by default).  Add properties to the new Animation using the dropdown menu.  Select Transform from the dropdown and click "Add".  To add keyframes, click on the new Transform button that appeared and click "Add Keyframe" just above the Animator Timeline.  To edit the new keyframe, be sure you have the Keyframe Editor open and click on the keyframe diamond pip in the Animation timeline.  You are free to drag each animation keyframe along the timeline grid to change the time the keyframe is played.  You can also zoom in and out in the timeline window for more precision.

In the Keyframe Editor, the properties you can animate for any component are listed with a checkbox next to them.  If you would like to animate a property you must check the box next to it to enable it.  Change the scale property from x = 1, y = 1 to x = 2, y = 2 and save the animation using the same expand button menu you used to load the Animation.

Now we have to attach the Animation to a GameObject.  Create a new GameObject with a Sprite component and add an image path to the Sprite by dragging one from the File Explorer viewport or by using the folder button next to the input.  Now add an Animation component to the GameObject and drag our new Animation into the input or click the folder button next to the input and give the Animation a name that we can use to reference it, then click "Add Animation" to officially add the animation to our GameObject.  Don't forget to save the scene using `File > Save Scene` ... or click on the expand button menu in the top right of the Hierarchy viewport and click "Save Scene" or "Save all".  Now that our GameObject has the Animation attached to it we can click "Preview" in the Animation component to preview the Animation on our GameObject in the SceneView while the game loop is running.

To call this animation and play it using scripts we can use:

<br/>`GameObject:GetAnimation():Play("nameOfAnimation")`<br/>

Where `nameOfAnimation` is the name we gave it in the Animation component of the GameObject.


____________________________________________________________________________________________________________

### Building The Final Project
To build a game Project, open the Project Settings by clicking `Settings -> Project Settings`:
____________________________________________________________________________________________________________
![navigateToProjectSettings](https://github.com/user-attachments/assets/ca6ce978-99fb-4b10-88cd-5e2e82909ca8)</br>
____________________________________________________________________________________________________________
Then make sure to set the `Game start Scene path` and the `Final project build path`.  The Game start Scene path must be relative to the FlatEngine-Runtime folder. ie. `../projects/project_name/scenes/scene_name.scn`.  The build path can be an absolute path.  Neither paths should contain quotation marks.
____________________________________________________________________________________________________________
![projectSettings](https://github.com/user-attachments/assets/20a7e9f8-efd4-4061-bbb5-a3ff6a51a34d)
____________________________________________________________________________________________________________
Then, you can build your final game by clicking the `Settings -> Build Project` button in the main menu bar.  FlatEngine2D will create a directory in the location you specified and move all the required files over.</br>
____________________________________________________________________________________________________________
![buildproject](https://github.com/user-attachments/assets/021dcf24-a5b5-4fec-a6e7-a38aa92c198c)
____________________________________________________________________________________________________________
If you've configured the settings correctly as stated above, you should end up with a folder containing something like this:</br>
____________________________________________________________________________________________________________
![finalBuildDirectory](https://github.com/user-attachments/assets/581323a7-6143-4d25-8cd9-356fa1c479c2)</br>
____________________________________________________________________________________________________________
Go into the `Runtime` folder and you'll see:</br>
____________________________________________________________________________________________________________
![executable](https://github.com/user-attachments/assets/8e2e5a40-425c-45bf-82ad-1d4c9a4adef4)</br>
____________________________________________________________________________________________________________
All of these files are important for your game to work properly except for the `log_output.txt` files and the `.pdb` files, which can safely be deleted.  You can now rename the `.exe` file to anything you want and run it.  Your game is now ready to be zipped up and distributed! Congratulations!
____________________________________________________________________________________________________________

That is the general intended work flow of FlatEngine2D. This will be updated as development progresses but I hope this is enough to get you started.  Read on for specifics about scripting in FlatEngine2D using Lua.


--------------------------------------------------------------------------------------



### Lua Scripting in FlatEngine

Please see the section at the very bottom where every engine and class function exposed to Lua is listed along with a description of what it does and what it returns.

Scripting in FlatEngine2D is done in a specific way due to how Lua is implemented in it.  Before each script is run, a Lua table is created for each Lua Script that exists.  These tables will be used by each script of the same name to keep track of and access each instance of the GameObject that "owns" a copy, using the GameObjects ID.  For example:

A Script named "Health" is created.  At time of creation, a new Lua table is made called Health.  Then, in the `Awake()` function of the `Health.scp.lua` script file, a new index in the Health table is created using the ID of the GameObject.  This means that each GameObject can only have ONE script component for each script file that exists.  You cannot have two script components that have the same Script attached in one GameObject (I am not sure why you would want to anyway).  IMPORTANT: All data that is specific to this script that needs to be tracked and accessed on a per-object basis MUST be put inside this table if you want to access it later in the script and from other script files.  This is because of the global nature of the Lua implementation. Every Script sees every other Script and every other function.

*Note: GameObject IDs can be viewed by hovering over a GameObject in the Hierarchies and holding `left alt`.  The same can be done to view the IDs of components in the Inspector View.*

Let us say that you want to track the health of whatever GameObject that is using this script.  In the `Awake()` function of this script you would add your health variables like so:


```
-- Health.scp.lua

function Awake() 
    Health[my_id] = 
    {
        totalHealth = 20,
        currentHealth = 20
    }        
end 
```


If you then needed to access these values for the specific GameObject, you need to access the Health table using the id of the specific GameObject you are interested in.  Now, there are two different ways a script can be accessed in FlatEngine:

1. Through the engine, either via the main script functions: Awake, Start, and Update, or through an Animation Event call.
2. From other scripts

When a script is called in the first way, the script is "initialized" by FlatEngine, which just means it sets some variables that you can access with Lua to specific values.  It sets the variable `this_object` to the GameObject that is calling the function, and it sets the variable `my_id` to the ID of the GameObject that is being called.  If the Script is called via the engine you can use the `my_id` variable in conjunction with the script table to access the data of the specific script instance (in the `Update()` function of the Health script, for example) like so:


```
-- Health.scp.lua

function Update()
    local data = Health[my_id]
    
    if data.currentHealth == 0 then
        -- do death things
    end
end

-- ignore this for now
function Damage(id, amount)
    local data = Health[id]
    data.currentHealth = data.currentHealth - amount
end
```


The data variable is a convenient way to not have to type `Health[my_id].currentHealth` every time and it is local because the default for Lua variables is global and we want to keep this variable only accessable to this Update function.


For the second way a script can be called, through another script, you must ensure that the ID you are referencing belongs to the script you actually want to perform actions on before you do so.  Let's say you have a BlasterRound script that when `OnBeginCollision` is called, it needs to tell whoever it collided with that it has done damage:


```
-- BlasterRound.scp.lua

function OnBeginCollision(collidedWith, manifold)
     local data = BlasterRound[my_id]

     if collidedWith:GetParent():HasTag("Enemy") then 
          local collidedID = collidedWith:GetParentID()         
          Damage(collidedID, 5)          
     end     
end
```


There is a lot going on here so let's break it down:

This function, `OnBeginCollision()`, is one of several functions that are called during specific events by FlatEngine.  It is called whenever the owning GameObject's collision body collides with another GameObject's.  Because it is a function that is called by the engine, it is guaranteed that the `my_id` and `this_object` variables will contain the data associated with this script's instance so we can freely use `my_id` to access it's data.  However, because we need to communicate with another script to tell it to do damage, we have to get ahold of that object's ID.  To do that we can use the `GetParentID()` function on the `Body* collidedWith` parameter that is passed into `OnBeginCollision()`.  `GetParentID()` is a function of the Component class and can be used on any Component to get the ID of the GameObject that owns it.  Let's continue:

`local data = BlasterRound[my_id]`<br/>
We saw this earlier. We are using the `my_id` variable to gain access to the script instances data and storing it in the local `data` variable.

`local collidedID = collidedWith:GetParentID()`<br/>
collidedWith is a of type `Body*`, which like all components, has a `GetParentID()` function. We assign the `Body*`'s parent ID to a local variable `collidedID`.

`if collidedWith:GetParent():HasTag("Enemy") then`<br/>
`GetParent()` is a Component function that gets the actual GameObject that owns this component.  `HasTag()` is a boolean function that checks for a specific Tag on a GameObject (not as important to this demonstration).

`Damage(collidedID, 5)`<br/>
This is a call to a function in another Script file.  The Damage function lives in the Health script file we saw above.  It takes two parameters: id and amount.  Because this function is intended to be called by other script files, we require an ID be passed to it to specify which script instance we are doing damage to.  From the BlasterRound script, we call the `Damage()` function using the ID of the GameObject that we want to do damage to along with the damage amount, 5.  This way the Health script knows the data it is operating on is the intended data.  If we were to have used the `my_id` variable inside the `Damage()` function like this (DON'T DO THIS):


```
function Damage(amount)
    -- DON'T DO THIS --
    local data = Health[my_id]
    data.currentHealth = data.currentHealth - amount
end
```


Then when the Damage function is called from `BlasterRound.lua.scp` in the `OnBeginCollision()` function, the engine will have assigned the variables `my_id` and `this_object` with references to the BlasterRound object (in this case), the `Damage()` function would be doing damage to the BlasterRound GameObject because it is accessing the Health table using it's ID.  That is also assuming there is any data to access in the first place, as the BlasterRound GameObject may not even have a Health script with data to access.

Hopefully this distinction makes sense because it is essential in understanding how Lua operates on GameObjects within FlatEngine.

If this doesn't make sense, I recommend looking into Lua and what is really happening when a Lua script file is opened.  The short of it is that the script files are just for show.  All Lua is run within the same environment so all of the functions in every script (except those that live inside of other functions) are accessable by all other functions in all other script files at any time.  So every time a script is loaded, it is just pooling the functions in that script into a table to be operated on within the same global environment as all the other scripts.  That is why we need to enclose the data specific to each object in a table that is only accessable via Script name tables with GameObject IDs as keys.  Otherwise there would be no way to know whose data any given function needs to access at any given moment.


Here is a list of every function that is called by the engine at specific times and therefore is guaranteed to have the my_id and this_object variables set to whatever GameObject is currently being referred to by the script:

`Awake()` -- Called upon instantiation of the GameObject</br>
`Start()` -- Called after all Awake functions have been called upon instantiation of the GameObject</br>
`Update()` -- Called once per frame</br>
`OnCollision(collidedWith, manifold)` -- Called every frame there is a collision happening on an object that has a Body that is colliding</br>
`OnBeginCollision(collidedWith, manifold)` -- Called on the first frame a collision happens on an object that has a Body that is colliding</br>
`OnEndCollision(collidedWith, manifold)` -- Called when a collision ceases on an object that has a Body that is now no longer colliding</br>
`OnButtonMouseOver()` -- Called every frame the mouse is hovering a Button</br>
`OnButtonMouseEnter()` -- Called on the first frame a mouse is hovering a Button</br>
`OnButtonMouseLeave()` -- Called when the mouse stops hovering a Button</br>
`OnButtonLeftClick()` -- Called when a Button is left clicked</br>
`OnButtonRightClick()` -- Called when a Button is right clicked</br>
Animation Events -- Any Animation Event function that is called during an Animation</br>



--------------------------------------------------------------------------------------



## Lua Functions Available

This is a comprehensive list of all functions that can be called in Lua to interact with FlatEngine2D, GameObjects, and their components:

`CreateGameObject(long parentID = -1)`<br/>
Action: Creates a GameObject.<br/>
Parameters: `long parentID = -1` - ID of the GameObject to parent it to. defaults to -1 by default for no parent.<br/>
Returns: `GameObject*` - pointer to the new GameObject<br/>

`CreateStringVector()`<br/>
Action: Creates a new `std::vector<std::string>()`.<br/>
Parameters: `None`<br/>
Returns: `std::vector<std::string>()`<br/>

`CreateIntVector()`<br/>
Action: Creates a new `std::vector<int>()`.<br/>
Parameters: `None`<br/>
Returns: `std::vector<int>()`<br/>

`CreateLongVector()`<br/>
Action: Creates a new `std::vector<long>()`.<br/>
Parameters: `None`<br/>
Returns: `std::vector<long>()`<br/>

`CreateFloatVector()`<br/>
Action: Creates a new `std::vector<float>()`.<br/>
Parameters: `None`<br/>
Returns: `std::vector<float>()`<br/>

`CreateDoubleVector()`<br/>
Action: Creates a new `std::vector<double>()`.<br/>
Parameters: `None`<br/>
Returns: `std::vector<double>()`<br/>

`CreateBoolVector()`<br/>
Action: Creates a new `std::vector<bool>()`.<br/>
Parameters: `None`<br/>
Returns: `std::vector<bool>()`<br/>

`IntToString(int value)`<br/>
Action: Converts an int to an std::string.<br/>
Parameters: `int value`<br/>
Returns: `std::string` - the converted value.<br/>

`LongToString(long value)`<br/>
Action: Converts a long to an std::string.<br/>
Parameters: `long value`<br/>
Returns: `std::string` - the converted value.<br/>

`FloatToString(float value)`<br/>
Action: Converts a float to an std::string.<br/>
Parameters: `float value`<br/>
Returns: `std::string` - the converted value.<br/>

`DoubleToString(double value)`<br/>
Action: Converts a double to an std::string.<br/>
Parameters: `double value`<br/>
Returns: `std::string` - the converted value.<br/>

`GetInstanceData(std::string scriptName, long ID)`<br/>
Action: Gets the data table associated with a given Script instance.<br/>
Parameters: `std::string scriptName, long ID` - ID: ID of the GameObject that owns the Script component.<br/>
Returns: `table instanceData` - a Lua table with the data.<br/>

`ContainsData(std::string scriptName, long ID)`<br/>
Action: Checks whether a Lua table of instance data exists for a specified Script name with specified GameObject ID.<br/>
Parameters: `std::string scriptName, long ID`<br/>
Returns: `bool` - Whether a table entry exists or not.<br/>

`GetScriptParam(std::string paramName, long ID, std::string scriptName = calling_script_name)`<br/>
Action: Gets the specified `ScriptParameter` from the `ParameterList` on a specified Script component owned by a GameObject.<br/>
Parameters: `std::string paramName, long ID, std::string scriptName`<br/>
Returns: `ScriptParameter` - The requested parameter or empty `ScriptParameter` with `type = empty` if it doesn't exist. `scriptName` defaults to the calling script.<br/>

`LoadGameObject(long ID)`<br/>
Action: Sets the specified GameObject as the loaded GameObject. This sets the `this_object` and `myID` Lua variables to that GameObject.<br/>
Parameters: `long ID` - The desired GameObjects ID.<br/>
Returns: `void`<br/>

`GetObjectByID(long ID)`<br/>
Action: Gets a GameObject based on ID.<br/>
Parameters: `long ID` - The desired GameObjects ID.<br/>
Returns: `GameObject*` - The GameObject with specified ID or nullptr if it doesn't exist.<br/>

`GetObjectByName(std::string objectName)`<br/>
Action: Gets a GameObject in the loaded scene by name.<br/>
Parameters: `std::string objectName` - name of the object you want.<br/>
Returns: `GameObject*`<br/>

`LoadScene(std::string sceneName)`<br/>
Action: Loads a scene<br/>
Parameters: `std::string sceneName` - name of the scene to load.<br/>
Returns: `void`<br/>

`ReloadScene()`<br/>
Action: Reload the currently loaded scene.<br/>
Parameters: `std::string sceneName` - name of the scene to reload.<br/>
Returns: `void`<br/>

`GetLoadedScene()`<br/>
Action: Gets the currently loaded scene.<br/>
Parameters: `none`<br/>
Returns: `Scene*` - The current Scene or `nullptr` if no Scene is loaded.<br/>

`LogString(std::string line)`<br/>
Action: Log a string to the console.<br/>
Parameters: `std::string line` - string to log<br/>
Returns: `void`<br/>

`LogInt(int value, std::string line = "")`<br/>
Action: Log an int to the console and a string that will be prefixed to the value.<br/>
Parameters: `int value` - value to log, line - (optional) string that will be prefixed to the value<br/>
Returns: `void`<br/>

`LogFloat(float value, std::string line = "")`<br/>
Action: Log an float to the console and a string that will be prefixed to the value.<br/>
Parameters: `float value` - value to log, line - (optional) string that will be prefixed to the value<br/>
Returns: `void`<br/>

`LogDouble(double value, std::string line = "")`<br/>
Action: Log an double to the console and a string that will be prefixed to the value.<br/>
Parameters: `double value` - value to log, line - (optional) string that will be prefixed to the value<br/>
Returns: `void`<br/>

`LogLong(long value, std::string line = "")`<br/>
Action: Log an long to the console and a string that will be prefixed to the value.<br/>
Parameters: `long value` - value to log, line - (optional) string that will be prefixed to the value<br/>
Returns: `void`<br/>

`GetMappingContext(std::string contextName)`<br/>
Action: Get a copy of a Mapping Context object by name.<br/>
Parameters: `std::string contextName` - Name of the Mapping Context<br/>
Returns: `MappingContext*` - or `nullptr` if none exists with that name.<br/>

`Instantiate(std::string prefabName, Vector2 position)`<br/>
Action: Instantiate a Prefab at a specific location<br/>
Parameters: prefabName - name of the Prefab to spawn, position - the position in the game world to spawn the Prefab.<br/>
Returns: `GameObject*`<br/>

`CloseProgram()`<br/>
Action: Closes the game and the editor.<br/>
Parameters: `none`<br/>
Returns: `void`<br/>

`SceneDrawLine(Vector2 startPoint, Vector2 endPoint, Vector4 color, float thickness)`<br/>
Action: Draws a debug line inside the Scene View.<br/>
Parameters: `Vector2 startPoint, Vector2 endPoint, Vector4 color, float thickness`<br/>
Returns: `void`<br/>

`SceneGameLine(Vector2 startPoint, Vector2 endPoint, Vector4 color, float thickness)`<br/>
Action: Draws a debug line inside the Game View.<br/>
Parameters: `Vector2 startPoint, Vector2 endPoint, Vector4 color, float thickness`<br/>
Returns: `void`<br/>

`GetTime()`<br/>
Action: Get the time in milliseconds the gameloop has been active (started and unpaused).<br/>
Parameters: none<br/>
Returns: Uint32<br/>

`Destroy(long ID)`<br/>
Action: Delete a GameObject by ID<br/>
Parameters: ID - ID of the GameObject to delete<br/>
Returns: void<br/>

`GetColor(std::string color)`<br/>
Action: Gets the Vector4 that represents a color in the Colors.lua file in the project directory<br/>
Parameters: color - name of the color in the Colors.lua file in FlatEngine -> engine -> scripts -> Colors.lua<br/>
Returns: Vector4<br/>

`RandomInt(int min, int max)`<br/>
Action: Get a random int between two values.<br/>
Parameters: `int min, int max`<br/>
Returns: `int`<br/>

`RandomInt(float min, float max)`<br/>
Action: Get a random float between two values.<br/>
Parameters: `float min, float max`<br/>
Returns: `float`<br/>

`Remap(std::string contextName, std::string inputAction, int timeoutTime)`<br/>
Action: Begins a timer that allows the next button press to map to the specified InputAction on the specified MappingContext.<br/>
Parameters: `std::string contextName`, `std::string inputAction` - InputAction to remap, `int timeoutTime` - how long in milliseconds to allow for input before timing out.<br/>
Returns: `void`<br/>

-----------------------------------------------------------

## FlatEngine classes exposed to Lua - Lua usertypes

## Class Vector2
|Method|Details|
|:-----|-------|
|`Vector2(), Vector2(float x,float y)`|Action: Constructors</br>Parameters: `float x, float y` - values to set x and y to.</br>\Returns: `Vector2`|
|`SetX(float x)`|Action: Sets the x-value of the Vector2.</br>Parameters: `float x` - the value to set x to.</br>Returns: `void`|
|`x()`|Action: Gets the x component.</br>Parameters: `none`</br>Returns: `float`|
|`SetY(float y)`|Action: Sets the y-value of the Vector2.</br>Parameters: `float y` - the value to set y to.</br>Returns: `void`|
|`y()`|Action: Gets the y component.</br>Parameters: `none`</br>Returns: `float`|
|`SetXY(float x, float y)`|Action: Sets both the x and y components.</br>Parameters: `float x, float y`</br>Returns: `none`|

## Class Vector4
|Method|Details|
|:-----|-------|
|`Vector4(), Vector4(float x, float y, float z, float w)`|Action: Constructors</br>Parameters: `float x, float y, float z, float w` - values to set x, y, z, and w to.</br>Returns: `Vector4`|
|`SetX(float x)`|Action: Sets the x-value of the Vector2.</br>Parameters: `float x` - the value to set x to.</br>Returns: `void`|
|`x()`|Action: Gets the x component.</br>Parameters: `none`</br>Returns: `float x`|
|`SetY(float y)`|Action: Sets the y-value of the Vector2.</br>Parameters: `float y` - the value to set y to.</br>Returns: `void`|
|`y()`|Action: Gets the y component.</br>Parameters: `none`</br>Returns: `float y`|
|`SetZ()`|Action: Sets the z component.</br>Parameters: </br>Returns: `void`| 
|`z()`|Action: Gets the z component.</br>Parameters: </br>Returns: `float z`|
|`SetW()`|Action: Sets the w component.</br>Parameters: </br>Returns: `void`| 
|`w()`|Action: Gets the w component.</br>Parameters: </br>Returns: `float w`|
|`SetXYZW()`|Action: Sets all four components.</br>Parameters: </br>Returns: `void`|

## Class Scene
|Method|Details|
|:-----|-------|
|`SetName(std::string name)`|Action: Sets the name of calling Scene.</br>Parameters: `std::string name`</br>Returns: `void`|
|`GetName()`|Action: Gets the name of calling Scene.</br>Parameters: `none`</br>Returns: `std::string`|
|`GetPath()`|Action: Returns the filepath of the calling Scene.</br>Parameters: `none`</br>Returns: `std::string`|

## Class GameObject
|Method|Details|
|:-----|-------|
|`GetID()`|Action: Gets GameObject ID.</br>Parameters: `none`</br>Returns: `long`|
|`GetName()`|Action: Gets GameObject name.</br>Parameters: `none`</br>Returns: `std::string`|
|`SetName(std::string name)`|Action: Sets GameObject name.</br>Parameters: `std::string name`</br>Returns: `void`|
|`IsActive()`|Action: Returns whether the GameObject is set to Active.</br>Parameters: `none`</br>Returns: `bool`|
|`SetActive(bool isActive)`|Action: Sets the GameObject as active or inactive.</br>Parameters: `bool isActive`</br>Returns: `void`|
|`GetParent()`|Action: Gets the GameObjects parent.</br>Parameters: `none`</br>Returns: `GameObject` or `nullptr` if it doesn't have one.|
|`GetParentID()`|Action: Gets the GameObjects parent ID.</br>Parameters: `none`</br>Returns: `long`|
|`HasTag(std::string tag)`|Action: Returns whether the GameObject has a Tag.</br>Parameters: `std::string tag` </br>Returns: `bool`|
|`SetTag(std::string tag, bool hasTag)`|Action: Sets specified Tag for the GameObject.</br>Parameters: `std::string tag, bool hasTag`</br>Returns: `void`|
|`SetIgnore(std::string, bool ignores)`|Action: Sets specified Tag for the GameObject to ignore.</br>Parameters: `std::string ignoreTag, bool ignores`</br>Returns: `void`|
|`GetTransform()`|Action: Gets the Transform Component.</br>Parameters: `none`</br>Returns: `Transform*` or `nullptr`|
|`GetSprite()`|Action: Gets the Sprite Component.</br>Parameters: `none`</br>Returns: `Sprite*` or `nullptr`|
|`GetCamera()`|Action: Gets the Camera Component.</br>Parameters: `none`</br>Returns: `Camera*` or `nullptr`|
|`GetScript()`|Action: Gets the Script Component.</br>Parameters: `none`</br>Returns: `Script*` or `nullptr`|
|`GetAnimation()`|Action: Gets the Animation Component.</br>Parameters: `none`</br>Returns: `Animation*` or `nullptr`|
|`GetAudio()`|Action: Gets the Audio Component.</br>Parameters: `none`</br>Returns: `Audio*` or `nullptr`|
|`GetButton()`|Action: Gets the Button Component.</br>Parameters: `none`</br>Returns: `Button*` or `nullptr`|
|`GetCanvas()`|Action: Gets the Canvas Component.</br>Parameters: `none`</br>Returns: `Canvas*` or `nullptr`|
|`GetText()`|Action: Gets the Text Component.</br>Parameters: `none`</br>Returns: `Text*` or `nullptr`|
|`GetCharacterController()`|Action: Gets the CharacterController Component.</br>Parameters: `CharacterController`</br>Returns: `CharacterController*` or `nullptr`|
|`GetRigidBody()`|Action: Gets the RigidBody Component.</br>Parameters: `none`</br>Returns: `RigidBody*` or `nullptr`|
|`GetBoxCollider()`|Action: Gets the BoxCollider Component.</br>Parameters: `none`</br>Returns: `BoxCollider*` or `nullptr`|
|`GetCircleCollider()`|Action: Gets the CircleCollider Component.</br>Parameters: `none`</br>Returns: `CircleCollider*` or `nullptr`|
|`GetTileMap()`|Action: Gets the TileMap Component.</br>Parameters: `none`</br>Returns: `TileMap*` or `nullptr`|
|`AddSprite()`|Action: Adds and returns a Sprite component to the GameObject if one doesn't already exist.</br>Parameters: `none`</br>Returns: `Sprite*` or `nullptr`|
|`AddScript()`|Action: Adds and returns a Script component to the GameObject.</br>Parameters: `none`</br>Returns: `Script*` or `nullptr`|
|`AddCamera()`|Action: Adds and returns a Camera component to the GameObject if one doesn't already exist.</br>Parameters: `none`</br>Returns: `Camera*` or `nullptr`|
|`AddAnimation()`|Action: Adds and returns an Animation component to the GameObject if one doesn't already exist.</br>Parameters: `none`</br>Returns: `Animation*` or `nullptr`|
|`AddAudio()`|Action: Adds and returns an Audio component to the GameObject if one doesn't already exist.</br>Parameters: `none`</br>Returns: `Audio*` or `nullptr`|
|`AddButton()`|Action: Adds and returns a Button component to the GameObject if one doesn't already exist.</br>Parameters: `none`</br>Returns: `Button*` or `nullptr`|
|`AddCanvas()`|Action: Adds and returns a Canvas component to the GameObject if one doesn't already exist.</br>Parameters: `none`</br>Returns: `Canvas*` or `nullptr`|
|`AddText()`|Action: Adds and returns a Text component to the GameObject if one doesn't already exist.</br>Parameters: `none`</br>Returns: `Text*` or `nullptr`|
|`AddCharacterController()`|Action: Adds and returns a CharacterController component to the GameObject if one doesn't already exist.</br>Parameters: `none`</br>Returns: `CharacterController*` or `nullptr`|
|`AddBoxBody()`|Action: Adds and returns a BoxBody component to the GameObject if one doesn't already exist.</br>Parameters: `none`</br>Returns: `BoxBody*` or `nullptr`|
|`AddCircleBody()`|Action: Adds and returns a CircleCollider component to the GameObject if one doesn't already exist.</br>Parameters: `none`</br>Returns: `CircleBody*` or `nullptr`|
|`AddTileMap()`|Action: Adds and returns a TileMap component to the GameObject if one doesn't already exist.</br>Parameters: `none`</br>Returns: `TileMap*` or `nullptr`|
|`AddChild(long childID)`|Action: Adds specified child to GameObject.</br>Parameters: `long childID`</br>Returns: `void`|
|`RemoveChild()`|Action: Removes specified child from GameObject.</br>Parameters: `long childID`</br>Returns: `void`|
|`GetFirstChild()`|Action: Gets the first child found on a GameObject. Not necessarily the first in the Hierarchy.</br>Parameters: </br>Returns: |
|`HasChildren()`|Action: Returns whether the GameObject has children.</br>Parameters: `none`</br>Returns: `bool hasChildren`|
|`GetChildren()`|Action: Gets the GameObjects children.</br>Parameters: `none`</br>Returns: `std::vector<long>` - IDs of the children.|
|`HasScript()`|Action: Returns whether a GameObject Has a Script with specified name.</br>Parameters: `std::string scriptName`</br>Returns: `bool hasScript`|
|`FindChildByName(std::string childName)`|Action: Gets specified child of GameObject by name.</br>Parameters: `std::string childName`</br>Returns: `GameObject*` or `nullptr` if no child exists with that name.|

## Class Transform
|Method|Details|
|:-----|-------|
|`GetParent()`|Action: Gets the GameObject that the component is attached to.</br>Parameters: `none`</br>Returns: `GameObject*`|
|`GetParentID()`|Action: Gets the ID of the GameObject the component is attached to.</br>Parameters: `none`</br>Returns: `long ID`|
|`GetID()`|Action: Gets the ID of the component.</br>Parameters: `none`</br>Returns: `long ID`|
|`SetPosition(Vector2 scale)`|Action: Sets the position of the GameObject.</br>Parameters: `Vector2 position`</br>Returns: `void`|
|`GetPosition()`|Action: Gets the GameObjects current position relative to it's origin.</br>Parameters: `none`</br>Returns: `Vector2 relativePosition`|
|`GetTruePosition()`|Action: Gets the GameObjects current position relative to the center point of the Scene.</br>Parameters: `none`</br>Returns: `Vector2 truePosition`|
|`SetRotation(float rotation)`|Action: Sets the rotation in degrees of the GameObjects. Only effects Sprites currently.</br>Parameters: `float rotation`</br>Returns: `void`|
|`GetRotation()`|Action: Gets the rotation in degrees of the GameObject.</br>Parameters: `none`</br>Returns: `float rotation`|
|`SetScale(Vector2 scale)`|Action: Sets the x and y scale of the GameObject. Should effect all components.</br>Parameters: `Vector2 scale`</br>Returns: `void`|
|`GetScale()`|Action: Gets the x and y scale of the GameObject.</br>Parameters: `none`</br>Returns: `Vector2 scale`|
|`LookAt()`|Action: Causes the GameObject to rotate at the specified position.</br>Parameters: `Vector2 position`</br>Returns: `void`|

## Class Sprite
|Method|Details|
|:-----|-------|
|`GetParent()`|Action: Gets the GameObject that the component is attached to.</br>Parameters: `none`</br>Returns: `GameObject*`|
|`GetParentID()`|Action: Gets the ID of the GameObject the component is attached to.</br>Parameters: `none`</br>Returns: `long ID`|
|`GetID()`|Action: Gets the ID of the component.</br>Parameters: `none`</br>Returns: `long ID`|
|`SetActive(bool isActive)`|Action: Sets the component as active or inactive.</br>Parameters: `bool isActive`</br>Returns: `void`|
|`IsActive()`|Action: Returns whether the component is active.</br>Parameters: `none`</br>Returns: `bool isActive`|
|`SetTexture(std::string texturePath)`|Action: Sets the path of the Sprites texture.</br>Parameters: `std::string texturePath`</br>Returns: `void`|
|`GetPath()`|Action: Gets the path of the Sprites texture.</br>Parameters: `none`</br>Returns: `std::string texturePath`|
|`SetScale(Vector2 scale)`|Action: Sets the x and y scale of the Sprites texture independantly and in addition to the Transform's scale.</br>Parameters: `Vector2 scale`</br>Returns: `void`|
|`GetScale()`|Action: Gets the x and y scale of the Sprites texture.</br>Parameters: `none`</br>Returns: `Vector2 scale`|
|`GetTextureWidth()`|Action: Gets the actual width of the texture in pixels.</br>Parameters: `none`</br>Returns: `int pixelWidth`|
|`GetTextureHeight()`|Action: Gets the actual height of the texture in pixels.</br>Parameters: `none`</br>Returns: `int pixelHeight`|
|`SetTintColor(Vector4 tintColor)`|Action: Sets the RGBA color to tint the Sprites texture.</br>Parameters: `Vector4 tintColor` - Vector4(x = red, y = green, z = blue, w = alpha)</br>Returns: `void`|
|`GetTintColor()`|Action: Gets the RGBA tintColor of the Sprites texture.</br>Parameters: `none`</br>Returns: `Vector4 tintColor` - Vector4(x = red, y = green, z = blue, w = alpha)|
|`SetAlpha(float alpha)`|Action: Set the alpha channel of the texture's tint color.</br>Parameters: `float alpha`</br>Returns: `void`|
|`GetAlpha()`|Action: Gets the value of the alpha channel of the texture's tint color.</br>Parameters: `none`</br>Returns: `float alpha`|
|`SetPivotPoint()`|Action: Sets the location the texture should pivot on when rotating.</br>Parameters: `std::string pivotPoint` - syntax expected for pivot point selection:Parameters: `std::string pivotPoint` - string syntax expected for pivot point selection: "PivotCenter", "PivotLeft", "PivotRight", "PivotTop", "PivotBottom", "PivotTopLeft", "PivotTopRight", "PivotBottomRight","PivotBottomLeft"</br>Returns: `void`|

## Class Text
|Method|Details|
|:-----|-------|
|`GetParent()`|Action: Gets the GameObject that the component is attached to.</br>Parameters: `none`</br>Returns: `GameObject*`|
|`GetParentID()`|Action: Gets the ID of the GameObject the component is attached to.</br>Parameters: `none`</br>Returns: `long ID`|
|`GetID()`|Action: Gets the ID of the component.</br>Parameters: `none`</br>Returns: `long ID`|
|`SetActive(bool isActive)`|Action: Sets the component as active or inactive.</br>Parameters: `bool isActive`</br>Returns: `void`|
|`IsActive()`|Action: Returns whether the component is active.</br>Parameters: `none`</br>Returns: `bool isActive`|
|`SetText()`|Action: Sets the text displayed by the Text component.</br>Parameters: `std::string displayedText`</br>Returns: `void`|
|`SetPivotPoint()`|Action: Sets the location the text should pivot on when rotating.</br>Parameters: `std::string pivotPoint` - string syntax expected for pivot point selection: "PivotCenter", "PivotLeft", "PivotRight", "PivotTop", "PivotBottom", "PivotTopLeft", "PivotTopRight", "PivotBottomRight","PivotBottomLeft"</br>Returns: `void`|

## Class Audio
|Method|Details|
|:-----|-------|
|`GetParent()`|Action: Gets the GameObject that the component is attached to.</br>Parameters: `none`</br>Returns: `GameObject*`|
|`GetParentID()`|Action: Gets the ID of the GameObject the component is attached to.</br>Parameters: `none`</br>Returns: `long ID`|
|`GetID()`|Action: Gets the ID of the component.</br>Parameters: `none`</br>Returns: `long ID`|
|`SetActive(bool isActive)`|Action: Sets the component as active or inactive.</br>Parameters: `bool isActive`</br>Returns: `void`|
|`IsActive()`|Action: Returns whether the component is active.</br>Parameters: `none`</br>Returns: `bool isActive`|
|`IsSoundPlaying()`|Action: Returns true if the specified sound is playing.</br>Parameters: </br>Returns: `bool isSoundPlaying`|
|`Play(std::string soundName)`|Action: Starts playing the specified sound attached to Audio component.</br>Parameters: `std::string soundName`</br>Returns: `void`|
|`Pause(std::string soundName)`|Action: Pauses the specified sound attached to Audio component.</br>Parameters: `std::string soundName`</br>Returns: `void`|
|`Stop(std::string soundName)`|Action: Stops playing the specified sound attached to Audio component.</br>Parameters: `std::string soundName`</br>Returns: `void`|
|`StopAll()`|Action: Stops all sounds that are currently playing on the Audio component.</br>Parameters: `none`</br>Returns: `void`|

## Class Button
|Method|Details|
|:-----|-------|
|`GetParent()`|Action: Gets the GameObject that the component is attached to.</br>Parameters: `none`</br>Returns: `GameObject*`|
|`GetParentID()`|Action: Gets the ID of the GameObject the component is attached to.</br>Parameters: `none`</br>Returns: `long ID`|
|`GetID()`|Action: Gets the ID of the component.</br>Parameters: `none`</br>Returns: `long ID`|
|`SetActive(bool isActive)`|Action: Sets the component as active or inactive.</br>Parameters: `bool isActive`</br>Returns: `void`|
|`IsActive()`|Action: Returns whether the component is active.</br>Parameters: `none`</br>Returns: `bool isActive`|
|`SetActiveDimensions(float width, float height)`|Action: </br>Parameters: `float width, float height`</br>Returns: `void`|
|`SetActiveOffset(Vector2 activeOffset)`|Action: Set the amount in x and y that the Button is offset from it's Transform's position.</br>Parameters: `Vector2 activeOffset`</br>Returns: `void`|
|`GetActiveOffset()`|Action: Gets the anount in x and y the Button is offset from it's Transform's position.</br>Parameters: `none`</br>Returns: `Vector2 activeOffset`|
|`SetActiveLayer(float activeLayer)`|Action: Sets the Canvas layer the Button is active on and thus the priority it has over other Buttons on lower layers.</br>Parameters: `float activeLayer`</br>Returns: `void`|
|`GetActiveLayer()`|Action: Gets the Buttons active Canvas layer.</br>Parameters: `none`</br>Returns: `void`|
|`GetActiveWidth()`|Action: Gets the active width of the Button.</br>Parameters: `none`</br>Returns: `float activeWidth`|
|`GetActiveHeight()`|Action: Gets the active height of the Button.</br>Parameters: `none`</br>Returns: `float activeHeight`|
|`MouseIsOver()`|Action: Returns true if the mouse is hovering the button and can be clicked.</br>Parameters: `none`</br>Returns: `bool mouseIsOver`|
|`SetLeftClick(bool leftClickable)`|Action: Set whether left clicking can activate the Button.</br>Parameters: `bool leftClickable`</br>Returns: `void`|
|`GetLeftClick()`|Action: Returns true if the Button can be activated by a left click of the mouse.</br>Parameters: `none`</br>Returns: `bool leftClickable`|
|`SetRightClick(bool rightClickable)`|Action: Set whether right clicking can activate the Button.</br>Parameters: `bool rightClickable`</br>Returns: `void`|
|`GetRightClick()`|Action: Returns true if the Button can be activated by a right click of the mouse.</br>Parameters: `none`</br>Returns: `bool rightClickable`|
|`SetLuaFunctionName(std::string functionName)`|Action: Set the Lua function name that will be called when the Button is activated.</br>Parameters: `std::string functionName` </br>Returns: `void`|
|`GetLuaFunctionName()`|Action: Get the Lua function name that will be called when the Button is activated.</br>Parameters: `none`</br>Returns: `std::string functionName`|
|`SetLuaFunctionParams(ParameterList functionParameters)`|Action: Set the list of function parameters, (ParameterList), that will be passed to the Button when it is activated.</br>Parameters: `ParameterList functionParameters`</br>Returns: `void`|

## Class Script
|Method|Details|
|:-----|-------|
|`GetParent()`|Action: Gets the GameObject that the component is attached to.</br>Parameters: `none`</br>Returns: `GameObject*`|
|`GetParentID()`|Action: Gets the ID of the GameObject the component is attached to.</br>Parameters: `none`</br>Returns: `long ID`|
|`GetID()`|Action: Gets the ID of the component.</br>Parameters: `none`</br>Returns: `long ID`|
|`SetAttachedScript(std::string attachedScriptName)`|Action: Sets the Script that is attached to the Script component that will run during gameplay.</br>Parameters: `std::string scriptName`</br>Returns: `void`|
|`GetAttachedScript()`|Action: Gets the Script that is attached to the Script component.</br>Parameters: `none`</br>Returns: `std::string attachedScriptName`|
|`RunAwakeAndStart()`|Action: Run the Awake() and Start() Lua methods on the attached Script.</br>Parameters: `none`</br>Returns: `void`|

## Class Animation
|Method|Details|
|:-----|-------|
|`GetParent()`|Action: Gets the GameObject that the component is attached to.</br>Parameters: `none`</br>Returns: `GameObject*`|
|`GetParentID()`|Action: Gets the ID of the GameObject the component is attached to.</br>Parameters: `none`</br>Returns: `long ID`|
|`GetID()`|Action: Gets the ID of the component.</br>Parameters: `none`</br>Returns: `long ID`|
|`SetActive(bool isActive)`|Action: Sets the component as active or inactive.</br>Parameters: `bool isActive`</br>Returns: `none`|
|`IsActive()`|Action: Returns whether the component is active.</br>Parameters: `none`</br>Returns: `bool isActive`|
|`Play(std::string animationName)`|Action: Play the specified Animation on the Animation component.</br>Parameters: `std::string animationName`</br>Returns: `void`|
|`Stop(std::string animationName)`|Action: Stop the specified Animation on the Animation component.</br>Parameters: `std::string animationName`</br>Returns: `void`|
|`StopAll()`|Action: Stops all Animations that are currently playing on the Animation component.</br>Parameters: `none`</br>Returns: `void`|
|`IsPlaying(std::string animationName)`|Action: Returns true if the specified Animation is playing.</br>Parameters: `std::string animationName`</br>Returns: `bool isPlaying`|
|`HasAnimation(std::string animationName)`|Action: Returns true if the Animation component has an Animation with specified name.</br>Parameters: `std::string animationName`</br>Returns: `bool hasAnimation`|

## Class ScriptParameter
|Method|Details|
|:-----|-------|
|`type()`|Action: Gets the type of the ScriptParameter. ("string", "int", "float", "double", "long", "bool", "Vector2") </br>Parameters: `none`</br>Returns: `std::string type`|
|`SetType()`|Action: Sets the type of the parameter.</br>Parameters: `std::string type` - Can be "string", "int", "float", "double", "long", "bool", "Vector2"</br>Returns: `void`|
|`string()`|Action: Gets the string value inside the ScriptParameter.</br>Parameters: `none`</br>Returns: `std::string stringValue`|
|`int()`|Action: Gets the string value inside the ScriptParameter. </br>Parameters: `none`</br>Returns: `int intValue`|
|`long()`|Action: Gets the long value inside the ScriptParameter. </br>Parameters: `none`</br>Returns: `long longValue`|
|`float()`|Action: Gets the float value inside the ScriptParameter. </br>Parameters: `none`</br>Returns: `float floatValue`|
|`double()`|Action: Gets the double value inside the ScriptParameter. </br>Parameters: `none`</br>Returns: `double doubleValue`|
|`bool()`|Action: Gets the bool value inside the ScriptParameter. </br>Parameters: `none`</br>Returns: `bool boolValue`|
|`Vector2()`|Action: Gets the Vector2 value inside the ScriptParameter. </br>Parameters: `none`</br>Returns: `Vector2 vector2Value`|
|`SetString()`|Action: Sets the string value inside the ScriptParameter.</br>Parameters: `std::string stringValue`</br>Returns: `none`|
|`SetInt()`|Action: Sets the int value inside the ScriptParameter.</br>Parameters: `int intValue`</br>Returns: `none`|
|`SetLong()`|Action: Sets the long value inside the ScriptParameter.</br>Parameters: `long longValue`</br>Returns: `none`|
|`SetFloat()`|Action: Sets the float value inside the ScriptParameter.</br>Parameters: `float floatValue`</br>Returns: `none`|
|`SetDouble()`|Action: Sets the double value inside the ScriptParameter.</br>Parameters: `double doubleValue`</br>Returns: `none`|
|`SetBool()`|Action: Sets the bool value inside the ScriptParameter.</br>Parameters: `bool boolValue`</br>Returns: `none`|
|`SetVector2()`|Action: Sets the Vector2 value inside the ScriptParameter.</br>Parameters: `Vector2 vector2Value`</br>Returns: `none`|

## Class ParameterList
|Method|Details|
|:-----|-------|
|`SetParameters(std::vector<ScriptParameter> parameters)`|Action: Sets the ScriptParameters member variable of the ParameterList.</br>Parameters: `std::vector<ScriptParameter> parameters`</br>Returns: `void`|
|`AddParameter(ScriptParameter parameters)`|Action: Add a the ScriptParameter to the ParameterList.</br>Parameters: `ScriptParameter parameter`</br>Returns: `void`|


## Class RigidBody

NOTE: RigidBody has been combined with Colliders to form BoxBody, CircleBody, CapsuleBody, PolygonBody, ChainBody, which encapsulate the new Box2D physics and collision implementation.  Documentation coming soon.

|Method|Details|
|:-----|-------|
|`GetParent()`|Action: Gets the GameObject that the component is attached to.</br>Parameters: `none`</br>Returns: `GameObject*`|
|`GetParentID()`|Action: Gets the ID of the GameObject the component is attached to.</br>Parameters: `none`</br>Returns: `long ID`|
|`GetID()`|Action: Gets the ID of the component.</br>Parameters: `none`</br>Returns: `long ID`|
|`SetActive(bool isActive)`|Action: Sets the component as active or inactive.</br>Parameters: `bool isActive`</br>Returns: `none`|
|`IsActive()`|Action: Returns whether the component is active.</br>Parameters: `none`</br>Returns: `bool isActive`|
|`SetMass(float mass)`|Action: </br>Parameters: </br>Returns: `void`|
|`GetMass()`|Action: </br>Parameters: `none`</br>Returns: `float mass`|
|`SetRisingGravity(float risingGravity)`|Action: Sets the gravity experienced by the RigidBody in the positive y-direction.</br>Parameters: `float risingGravity`</br>Returns: `void`|
|`GetRisingGravity()`|Action: Gets the gravity experienced by the RigidBody in the positive y-direction.</br>Parameters: `none`</br>Returns: `float risingGravity`|
|`SetFallingGravity(float fallingGravity)`|Action: Sets the gravity experienced by the RigidBody in the negative y-direction.</br>Parameters: `float fallingGravity`</br>Returns: `void`|
|`GetFallingGravity()`|Action: Gets the gravity experienced by the RigidBody in the negative y-direction.</br>Parameters: `none`</br>Returns: `float fallingGravity`|
|`SetFriction(float friction)`|Action: Sets the ground friction experienced by the RigidBody.</br>Parameters: `float friction`</br>Returns: `void`|
|`GetFriction()`|Action: Gets the ground friction experienced by the RigidBody.</br>Parameters: `none`</br>Returns: `float friction`|
|`SetAngularDrag(float angularDrag)`|Action: Sets the rotational drag experienced by the RigidBody.</br>Parameters: `float angularDrag`</br>Returns: `void`|
|`GetAngularDrag()`|Action: Gets the rotational drag experienced by the RigidBody.</br>Parameters: `none`</br>Returns: `float angularDrag`|
|`SetAngularVelocity(float angularVelocity)`|Action: Set the angular velocity of the RigidBody.</br>Parameters: `float angularVelocity`</br>Returns: `void`|
|`GetAngularVelocity()`|Action: Gets the angularVelocity of the RigidBody.</br>Parameters: `none`</br>Returns: `float angularVelocity`|
|`SetTorquesAllowed(bool torquesAllowed)`|Action: Sets whether the RigidBody can experience torques (not tied to velocity).</br>Parameters: `bool torquesAllowed`</br>Returns: `void`|
|`TorquesAllowed()`|Action: Returns whether the RigidBody can experience torques.</br>Parameters: `none`</br>Returns: `bool torquesAllowed`|
|`AddForce(Vector2 forceDirection, float scale)`|Action: Add a force to the RigidBody in specified direction multiplied by scale.</br>Parameters: `Vector2 forceDirection, float scale`</br>Returns: `void`|
|`AddTorque(float torque, float scale)`|Action: Add a rotational force to the RigidBody.</br>Parameters: `float torque, float scale` - torque can be negative or positive depending on desired direction.</br>Returns: `void`|
|`GetVelocity()`|Action: </br>Parameters: </br>Returns: |
|`SetPendingForces()`|Action: Sets the total combined force that will be applied to the RigidBody this frame.</br>Parameters: Vector2 pendingForces</br>Returns: ``void`|
|`GetPendingForces()`|Action: Gets the total combined force that will be applied to the RigidBody this frame.</br>Parameters: `none`</br>Returns: Vector2 pendingForces|
|`SetTerminalVelocity(float terminalVelocity)`|Action: Sets the terminal velocity of the RigidBody.</br>Parameters: `float terminalVelocity`</br>Returns: `void`|
|`GetTerminalVelocity()`|Action: Gets the terminal velocity of the RigidBody.</br>Parameters:  `none`</br>Returns: `float terminalVelocity`|
|`IsGrounded()`|Action: Returns whether the GameObject is grounded. (uses attached BoxCollider to determine)</br> Parameters: `none`</br> Returns: `bool isGrounded`|

## Class Collider
|Method|Details|
|:-----|-------|
|`GetParent()`|Action: Gets the GameObject that the component is attached to.</br>Parameters: `none`</br>Returns: `GameObject*`|
|`GetParentID()`|Action: Gets the ID of the GameObject the component is attached to.</br>Parameters: `none`</br>Returns: `long ID`|
|`GetID()`|Action: Gets the ID of the component.</br>Parameters: `none`</br>Returns: `long ID`|
|`SetActive(bool isActive)`|Action: Sets the component as active or inactive.</br>Parameters: `bool isActive`</br>Returns: `none`|
|`IsActive()`|Action: Returns whether the component is active.</br>Parameters: `none`</br>Returns: `bool isActive`|

## Class BoxCollider - inherits from Collider
|Method|Details|
|:-----|-------|
|`GetParent()`|Action: Gets the GameObject that the component is attached to.</br>Parameters: `none`</br>Returns: `GameObject*`|
|`GetParentID()`|Action: Gets the ID of the GameObject the component is attached to.</br>Parameters: `none`</br>Returns: `long ID`|
|`GetID()`|Action: Gets the ID of the component.</br>Parameters: `none`</br>Returns: `long ID`|
|`SetActive(bool isActive)`|Action: Sets the component as active or inactive.</br>Parameters: `bool isActive`</br>Returns: `none`|
|`IsActive()`|Action: Returns whether the component is active.</br>Parameters: `none`</br>Returns: `bool isActive`|

## Class CircleCollider - inherits from Collider
|Method|Details|
|:-----|-------|
|`GetParent()`|Action: Gets the GameObject that the component is attached to.</br>Parameters: `none`</br>Returns: `GameObject*`|
|`GetParentID()`|Action: Gets the ID of the GameObject the component is attached to.</br>Parameters: `none`</br>Returns: `long ID`|
|`GetID()`|Action: Gets the ID of the component.</br>Parameters: `none`</br>Returns: `long ID`|
|`SetActive(bool isActive)`|Action: Sets the component as active or inactive.</br>Parameters: `bool isActive`</br>Returns: `none`|
|`IsActive()`|Action: Returns whether the component is active.</br>Parameters: `none`</br>Returns: `bool isActive`|

## Class CharacterController
|Method|Details|
|:-----|-------|
|`GetParent()`|Action: Gets the GameObject that the component is attached to.</br>Parameters: `none`</br>Returns: `GameObject*`|
|`GetParentID()`|Action: Gets the ID of the GameObject the component is attached to.</br>Parameters: `none`</br>Returns: `long ID`|
|`GetID()`|Action: Gets the ID of the component.</br>Parameters: `none`</br>Returns: `long ID`|
|`SetActive(bool isActive)`|Action: Sets the component as active or inactive.</br>Parameters: `bool isActive`</br>Returns: `none`|
|`IsActive()`|Action: Returns whether the component is active.</br>Parameters: `none`</br>Returns: `bool isActive`|
|`MoveToward(Vector2 direction)`|Action: Causes the GameObject to move in specified direction at the `maxSpeed` indicated inside the component settings.</br>Parameters: `Vector2 direction`</br>Returns: `void`|

## Class InputMapping
|Method|Details|
|:-----|-------|
|`KeyCode()`|Action: Gets the key code associated with the InputMapping.</br>Parameters: `none`</br>Returns: `std::string keyCode`|
|`InputActionName()`|Action: Gets the name of the InputAction associated with the InputMapping.</br>Parameters: `none`</br>Returns: `std::string inputActionName`|

## Class MappingContext
|Method|Details|
|:-----|-------|
|`Fired()`|Action: Returns true on initial press of InputAction key code.</br>Parameters: `none`</br>Returns: `bool inputActionFired`|
|`ActionPressed()`|Action: Returns true if the key code for the InputAction is currently being pressed.</br>Parameters: `none`</br>Returns: `bool inputActionPressed`|
|`GetName()`|Action: Gets the name of the MappingContext.</br>Parameters: `none`</br>Returns: `std::sting name`|
|`GetInputMappings()`|Action: Gets the InputMappings associated with the MappingContext.</br>Parameters: `none`</br>Returns: `std::vector<std::shared_ptr<InputMapping>> inputMappings`|


--------------------------------------------------------------------------------------

### Editing Engine Colors and More
You may access and customize the colors of FlatEngine2D by opening and editing the file located at `engine/scripts/Colors.lua`.</br>
You may access and customize the textures used by FlatEngine2D by opening and editing the file located at `engine/scripts/Textures.lua`.</br>
You may access and customize the Tags used by FlatEngine2D GameObjects by opening and editing the file located at `engine/scripts/Tags.lua`.
