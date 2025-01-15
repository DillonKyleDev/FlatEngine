# FlatEngine - 2D Game Engine


![ReleaseBuildV1](https://github.com/user-attachments/assets/e7a38d3e-963c-4bcb-a5f1-a01b33d6dda3)

## UPDATE

There is now a published release for FlatEngine2D!  Please see the "Releases" section on the right-hand side for a download link and read up on the ReadMe for details on how to use FlatEngine.

I've included a sample project inside FlatEngine2D called FlatSpace.  With it, hopefully you can get an idea of how to use FlatEngine.  But remember, FlatSpace is only one way to make a game using FlatEngine.  Get creative!  You are welcome to use any art assets I created that are in the release for anything you want.  There is a blue brick tileset in the /projects/FlatSpace/images/tileSets directory that can be used with the TileMap system to make levels, and in fact, there is a prefab in the prefabs directory called "LevelLayout.prf" that is a demo of this.  The audio is audio I purchased on the Unity asset store so that can't be used as far as I know, except to experiment with.

As a note, FlatSpace was made before the implementation of persistant GameObjects which enable you to have some GameObjects stick around even when changing scenes.  Creation of new persistant GameObjects is located in the Assets dropdown menu.  Persistant objects are tied to the Project, so make sure to save your Scenes AND your Projects often to keep your progress saved!

### Disclaimer

This project is in active development.

This is a hobby project.
It will likely crash eventually so save often. THERE IS NO AUTOSAVE.
Things will change with updates.
Your results and the usefulness of FlatEngine may vary.
Key systems are not optimized.

Many things in this README are out of date so keep that in mind when reading.  The general architecture of FlatEngine won't change that much over the course of development.


## About:

FlatEngine is a 2D game engine made using SDL2 and ImGui.  It is a passion project and a way to improve my software development skills. This game engine is intended to replace Unity in all my future 2D game development projects and maybe it will be useful to you as well.  I will also be using the base engine I develop here to create a full 3D game engine in the future.  See the license for details about using the engine and code, but as far as code I have written, you are free to use it as you wish, and you are also free to credit me if you would like to but it is not required.  Additionally, I would love to see any projects you are working on or have used any of the engine code for if you would be willing to share it.

I've spent a lot of time with the Unity game engine and so have found comfort in its design philosophy.  I will be adopting many of Unitys methods for handling various things within my engine.


--------------------------------------------------------------------------------------


## Libraries / Utilities

### SDL_2

The engine is built using SDL_2 as a base, along with several other SDL libraries including SDL_ttf for fonts, SDL_Image, and SDL_Mixer for audio.

### ImGui

The entire user interface, including all of the interactions within the scene view, are handled using ImGui_Docking.

### Json Formatter

Json Formatter is used for saving various types of data for later use including Scene data (GameObjects and components), Project data, and Animation data.

### Logging

For logging, I created my own library of functions that interface with ImGui.

### Scripting

Lua/Sol is the scripting language embedded in FlatEngine2D. Scripts are contained in files with the extension ".scp.lua" in order to be controlled as needed within the engine.

See "Using FlatEngine" below for a detailed walkthrough of how to use Lua in FlatEngine.


--------------------------------------------------------------------------------------


## Engine Components

### Tags

Tags are a list of properties that a GameObject can have that can be queried using 

`GameObject::HasTag("tagName")` 

The Tags system is also used in the collision detection system to prevent objects that should not interact from interacting, based on the tags each GameObject is set to Ignore.  Tags are completely customizeable by opening and editing the `Tags.lua` file found in `/engine/scripts/Tags.lua`.  Here you can freely add and remove Tags and they will appear in the engine upon either closing and reopening the engine or by clicking "Reload Tags" in the dropdown menu under `Settings -> Reload Tags` on the main menu bar.</br>

Tags must remain in the format present in the `Tags.lua` file: Comma-separated strings inside a Lua table named `F_Tags`

`F_Tags = {
  "Tagname",
  "Tagname2",
  "Tagname3"
}`

## Components

FlatEngines GameObjects require components be attached for their functionality.  The current list of components included in FlatEngine are:

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
11. BoxCollider
12. CircleCollider (work in progress -> CircleColliders currently do not support Solid collision and can only be used as triggers)
13. CompositeCollider (work in progress -> Currently removed from the engine)
14. RigidBody
15. CharacterController
16. TileMap (work in progress)


### Transform

All GameObjects MUST have a Transform and are created with one that cannot be removed.  Transforms handle:

| Property        | Description |
|:--------------------|:---------------|
|Origin| The reference point for all position changes.|
|Position| An (x,y) Vector2 that holds position relative to the Origin point.|
|Scale| The Scale of the entire object and its components (seperate from any additional component scale parameters, ie.. Sprite scale). Children are not yet affected by the scale of their parent. I am still working out how I want this aspect of scaling to be handled.|
|Rotation| The rotation in degrees of the object.  Currently the collision system does not account for rotations of objects so Sprites are all that are affected by this.|

### Sprite

Sprites are the visual representation of GameObjects in the scene.  Currently only PNG is supported.  Sprites have the followiing properties

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
|Is Music?| This may be removed in the future because I am not sure the end user should care about whether the Audio is music or a sound clip.. This is more of a backend option, but each are handled differently by SDL_Mixer so that will have to be sorted out eventually.|


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

The Collision components are not all complete.  The BoxCollider component is complete but it does not take rotation into account.  This is a feature I will be working on in the near future.  The CircleCollider can detect collisions, but it does not yet know how to handle positioning based on collisions.  The CompositeCollider I am waiting until the other Collision components are finished to continue work on it.  As a result, I have disabled the use of Circle and Composite colliders from the engine temporarily.  The BoxCollider uses a simple AABB collision detection approach.  The collision system is connected to the Tags system and will not check collisions between GameObjects that are ignoring each other based on the Tags they pocess.  The BoxCollider component has the following properties:

| Property        | Description |
|:--------------------|:---------------|
|Dimensions|The width and height of the collision box.|
|Offset|The position relative to the Transforms position|
|Active Layer|This feature is neglected due to the Tags system being in place, but it will be updated when I do the necessary overhaul of the collision system as a whole.  Colliders should only interact with other colliders on the same layer.|
|Is Continuous?|Determines how often this collider needs to be checked for collision. Currently it is set to every 10 frames for non continuous colliders, this is subject to change.|
|Is Static?|If a collider is static it may not need to be checked or updated as often as non static colliders, this saves on performance.|
|Is Solid?|Determines whether other collisiders should pass through this collider or if it is just a trigger. (Is Solid is disabled for CircleColliders at the moment but they can be used as triggers instead.)|
|Is Composite?|For adding this Collider to the CompositeCollider component on the GameObject (work in progress).|
|Show Active Radius?|Before the AABB testing occurs, each tested collision is tested using a less expensive radius check, this enables you to see that radius for each particular collider.|

### RigidBody

The RigidBody component is responsible for handling all of the phyiscs that an object can be subjected to.  Using the RigidBody, you can add forces and torques to an object to manipulate its velocity and rotation.  The RigidBody component has the following properties:

| Property        | Description |
|:--------------------|:---------------|
|Mass|Determines how difficult it is to change the velocity of the object.|
|Gravity| Determines how much gravity the GameObject experiences.|
|Falling Gravity| Falling gravity is used if you would like your GameObject to have a different gravity going down than it has going down.  This is useful for fine tuning the feeling of a jump or could be used to add a glide or hover mechanic.  Both this and regular Gravity Scale can be freely changed.|
|Terminal Velocity|The maximum speed a GameObject can fall due to gravity.|
|Wind Resistance|The amount that a GameObject is slowed while not grounded.|
|Friction|The amount that a GameObejct is slowed while grounded.|
|Angular Drag|The amount that a GameObject is slowed while rotating.|
|Equilibrium Force|I believe this will be removed in a future update.  I am unsure if this value should be exposed to the end user.|

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


--------------------------------------------------------------------------------------


## Engine Assets

<div>
  <h2>Projects</h2>
<picture>
 <img alt="Projects" src="https://github.com/user-attachments/assets/7fa21df7-cfc4-490e-8d17-836775eb90b5">
</picture>
<p>Projects contain all the data associated with a particular project including scene to load up at the start of a game but mostly information that is not relevant to the end user.</p>
</div>

<div>
  <h2>Scenes</h2>
<picture>
 <img alt="Scenes" src="https://github.com/user-attachments/assets/24bb752a-9bd9-45d1-937a-5692bfa1b1bc">
</picture>
<p>Scenes are the container for all GameObjects and are saved in JSON format using a JSON Formatting library for simple parsing.</p>
</div>

<div>
  <h2>TileSet</h2>
<picture>
 <img alt="TileSet" src="https://github.com/user-attachments/assets/33c561a0-40b5-4d0d-ab76-2a4ff9b8d828">
</picture>
<p>TileSets are used by the TileMap system to allow them to draw texture tiles.</p>
</div>

<div>
  <h2>Script</h2>
<picture>
 <img alt="Script" src="https://github.com/user-attachments/assets/9c2e9d4b-ab18-4624-b7ab-b213d16e306a">
</picture>
<p>Lua script files for scripting.  In FlatEngine, these files have the extension ".scp.lua".</p>
</div>

<div>
  <h2>Mapping Context</h2>
<picture>
 <img alt="Mapping Context" src="https://github.com/user-attachments/assets/fc6ed04f-5fb0-476d-a076-c1c7c1b9d181">
</picture>
<p>Mapping Contexts (and the Mapping Context Editor) are the interface for binding input actions to specific controls.  Currently mouse/keyboard and XInput (Xbox controller) are supported with more planned to be added in the future.  The Mapping Context system is not yet exposed to the Scripting system so it is not yet possible to dynamically set key bindings (in a game settings menu for example) but that is a feature that is easy to implement and is toward the top of the list of priorities.</p>
</div>

<div>
  <h2>Animation</h2>
<picture>
 <img alt="Animation" src="https://github.com/user-attachments/assets/e88f98cd-855e-40c8-8b32-e3759fcbdd08">
</picture>
<p>Animations are what the Animation components use to... play animations.  Animations are edited using the Animator window in combination with the Keyframe Editor.  If you have used Unity you should be familiar with how this functions.</p>
</div>

<div>
  <h2>Prefabs</h2>
<picture>
 <img alt="Prefab" src="https://github.com/user-attachments/assets/f0bb4978-2afe-425b-9ac0-6a09710e92ca">
</picture>
<p>Prefabs are saved GameObjects (including any children and components) that can be instantiated at runtime, or at any time.</p>
</div>



--------------------------------------------------------------------------------------


## Viewports - (Descriptions under construction)

### SceneView

Where your scenes are edited.

### GameView

Where you can see what the game will look like when it is run.

### Inspector

Where you edit and view GameObject Component data.

### Hierarchy

The list of all GameObjects in the loaded scene.

### Logger

Where all text output and error messages are displayed.

### Profiler (work in progress)

Where you can see how long processes take to complete.  This is currently not well-suited to be very useful to the end user and is in need of an overhaul to be made useful and extendable.  Here you can see collider pairs that the engine has made based on the Tags associated with each GameObject with a BoxCollider component.

### Animator

Used to animate components attached to the animated GameObject.

### Keyframe Editor

Used to edit the animated properties in an Animation.

### File Explorer

A real-time representation of the folder structure of the project directory.  Not feature complete but it supports basic creation of assets and deletion through a right-click context and drag and drop functionality into the SceneView to create object using assets and into the Inspector to attach references to images, audio files, animations to their respective components.

### Mapping Context Editor

Used to bind input actions to specific keys and buttons in a Mapping Context asset.

### TileSet Editor

Used to create TileSets that are used by the TileMap component.


--------------------------------------------------------------------------------------


## Using FlatEngine

WARNING: FlatEngine is NOT a complete engine and as such, I CAN GUARANTEE IT WILL CRASH and cause you to lose unsaved progress so SAVE YOUR SCENES OFTEN.  If you do experience a crash, which you will eventually, please do your best to recreate the crash and submit an issue in the repository with the details on how to recreate it.  That would be extremely helpful to me and I would greatly appreciate it.

With the warning out of the way... FlatEngine is intended to be vary straightforward to use and very fast to get up and running on a project.  

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

1. Hamburger button under the GameObjects name then click "Add Component"
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

You can then in a later function access the `mappingContext` variable to query whether an action.  There are two ways to query an input action:

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

From here you can open your Animation file using the hamburger menu in the Animator window. (If you double clicked it should be opened in the Animator by default).  Add properties to the new Animation using the dropdown menu.  Select Transform from the dropdown and click "Add".  To add keyframes, click on the new Transform button that appeared and click "Add Keyframe" just above the Animator Timeline.  To edit the new keyframe, be sure you have the Keyframe Editor open and click on the keyframe diamond pip in the Animation timeline.  You are free to drag each animation keyframe along the timeline grid to change the time the keyframe is played.  You can also zoom in and out in the timeline window for more precision.

In the Keyframe Editor, the properties you can animate for any component are listed with a checkbox next to them.  If you would like to animate a property you must check the box next to it to enable it.  Change the scale property from x = 1, y = 1 to x = 2, y = 2 and save the animation using the same hamburger menu you used to load the Animation.

Now we have to attach the Animation to a GameObject.  Create a new GameObject with a Sprite component and add an image path to the Sprite by dragging one from the File Explorer viewport or by using the folder button next to the input.  Now add an Animation component to the GameObject and drag our new Animation into the input or click the folder button next to the input and give the Animation a name that we can use to reference it, then click "Add Animation" to officially add the animation to our GameObject.  Don't forget to save the scene using `File > Save Scene` ... or click on the hamburger menu in the top right of the Hierarchy viewport and click "Quick Save".  Now that our GameObject has the Animation attached to it we can click "Preview" in the Animation component to preview the Animation on our GameObject in the SceneView.  Do keep in mind though that if you Play Animation while the gameloop is not running, the `Awake()` and `Start()` functions will not have been run on any scripts yet, so if you are using Event functions in your Animations, be mindful of that.  Also keep in mind that when you preview an animation, it changes the GameObjects properties and there is currently nothing built in to revert its properties to before you previewed it, so please make sure to save your changes before previewing animations, that way you can easily reload the scene to revert back to the original object properties.

To call this animation and play it using scripts we can use:

<br/>`GameObject:GetAnimation():Play("nameOfAnimation")`<br/>

Where `nameOfAnimation` is the name we gave it in the Animation component of the GameObject.


That is the general intended flow of using FlatEngine.  This concludes this brief introduction into using FlatEngine.  This will be updated as development progresses but I hope this is enough to get you started.  This project is in active development so there are a few things you need to keep in mind if you plan to use FlatEngine:

This is a hobby project.
It will crash eventually so save often. THERE IS NO AUTOSAVE.
Things will change, sometimes drastically, with updates.
Your results and the usefulness of FlatEngine may vary.



--------------------------------------------------------------------------------------



### Lua Scripting in FlatEngine

Please see the section at the very bottom where every function exposed to Lua will eventually be listed along with a description of what it does.

Scripting in FlatEngine is done in a specific way due to how Lua is implemented in it.  Before each script is run, a Lua table is created for each Lua Script that exists.  These tables will be used by each script of the same name to keep track of and access each instance of the GameObject that "owns" a copy, using the GameObjects ID.  For example:

A Script named "Health" is created.  At time of creation, a new Lua table is made called Health.  Then, in the `Awake()` function of the `PlayerController.scp.lua` script file, a new index in the Health table is created using the ID of the GameObject.  This means that each GameObject can only have ONE script component for each script that exists.  You cannot have two script components that have the same Script attached in one GameObject (I am not sure why you would want to anyway).  IMPORTANT: All data that is specific to this script that needs to be tracked and accessed on a per-object basis MUST be put inside this table if you want to access it later in the script and from other script files.

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

When a script is called in the first way, the script is "initialized" by FlatEngine, which just means it sets some variables that you can access with Lua to specific values.  It sets the variable `this_object` to the GameObject that is calling the function, and it sets the variable `my_id` to the ID of the GameObject that is being called.  If the Script is called via the engine you can use the my_id variable in conjunction with the script table to access the data of the specific script instance (in the `Update()` function of the Health script, for example) like so:


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


For the second way a script can be called, through another script, you must ensure that the ID you are referencing belongs to the script you actually want to perform actions on before you do so.  Let's say you have a BlasterRound script that when `OnBoxCollisionEnter` is called it needs to tell whoever it collided with that it has done damage:


```
-- BlasterRound.scp.lua

function OnBoxCollisionEnter(collidedWith)
     local data = BlasterRound[my_id]
     local collidedID = collidedWith:GetParentID()

     if collidedWith:GetParent():HasTag("Enemy") then          
          Damage(collidedID, 5)          
     end     
end
```


There is a lot going on here so let's break it down:

This function, `OnBoxCollisionEnter()`, is one of many functions that are called during specific events by FlatEngine.  It is called whenever this object collides with another GameObject.  Because it is a function that is called by the engine, it is guaranteed that the `my_id` and `this_object` variables will contain the data associated with this scripts instance so we can freely use `my_id` to access its data.  However, because we need to communicate with another script to tell it to do damage, we have to get ahold of that objects ID.  To do that we can use the `GetParentID()` function.  This function is a function of the Component class and can be used on any component to get the ID of the GameObject that owns it. Convenient!  Let's continue:

`local data = BlasterRound[my_id]`<br/>
We saw this earlier. We are using the `my_id` variable to gain access to the script instances data and storing it in the local data variable.

`local collidedID = collidedWith:GetParentID()`<br/>
collidedWith is a of type BoxCollider, which like all components, has a `GetParentID()` function. We assign the BoxColliders parent ID to a local variable collidedID.

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


Then when the Damage function is called from BlasterRound in the `OnBoxCollisionEnter()` function, the engine will have assigned the variables `my_id` and `this_object` with references to the BlasterRound object (in this case), the `Damage()` function would be doing damage to the BlasterRound GameObject because it is accessing the table using its ID.  That is also assuming there is any data to access in the first place, as the BlasterRound GameObject may not even have a Health script with data to access.

Hopefully this distinction makes sense because it is essential in understanding how Lua operates on GameObjects within FlatEngine.

If this doesn't make sense, I recommend looking into Lua and what is really happening when a Lua script file is opened.  The short of it is that the script files are just for show.  All Lua is run in the same environment so all of the functions in every script (except those that live inside other functions) are accessable by all other functions in all other script files at any time.  So every time a script is loaded, it is just pooling the functions in that script into a table to be operated on within the same global environment as all the other scripts.  That is why we need to enclose the data specific to each object in a table that is only accessable via Script names GameObject IDs as keys.  Otherwise there would be no way to know whose data any given function needs to access at any given moment.


Here is a list of every function that is called by the engine at specific times and therefore is guaranteed to have the my_id and this_object variables set to whatever GameObject is currently being referred to by the script:


`Awake()` -- Called at upon instantiation of the GameObject
`Start()` -- Called after all Awake functions have been called upon instantiation of the GameObject
`Update()` -- Called once per frame
`OnBoxCollision(collidedWith)` -- Called every frame there is a collision happening 
`OnBoxCollisionEnter(collidedWith)` -- Called on the first frame a collision happens
`OnBoxCollisionLeave(collidedWith)` -- Called when a collision ceases
`OnCircleCollision(collidedWith)` -- Called every frame there is a collision happening 
`OnCircleCollisionEnter(collidedWith)` -- Called on the first frame a collision happens
`OnCircleCollisionLeave(collidedWith)` -- Called when a collision ceases
`OnButtonMouseOver()` -- Called every frame the mouse is hovering a Button
`OnButtonMouseEnter()` -- Called on the first frame a mouse is hovering a Button
`OnButtonMouseLeave()` -- Called when the mouse stops hovering a Button
`OnButtonLeftClick()` -- Called when a Button is left clicked
`OnButtonRightClick()` -- Called when a Button is right clicked
Any Animation Event function that is called during an Animation



--------------------------------------------------------------------------------------



## Lua Functions Available

This is a comprehensive list of all functions that can be called in Lua to interact with FlatEngine2D, GameObjects, and components:

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


# UNDER CONSTRUCTION

### FlatEngine classes exposed to Lua and their methods (Lua usertypes)



## Scene

|Method|Details|
|:-----|-------|
|`SetName(std::string name)`|
Action: Sets the name of calling Scene.</br>
Parameters: `std::string name`</br>
Returns: `void`|
|`GetName()`|
Action: Gets the name of calling Scene.</br>
Parameters: `none`</br>
Returns: `std::string`|
|`GetPath()`|
Action: Returns the filepath of the calling Scene.</br>
Parameters: `none`</br>
Returns: `std::string`|

## Vector2

|Method|Details|
|:-----|-------|
|`Vector2(), Vector2(float x,float y)`|
Action: Constructors</br>
Parameters: `float x, float y` - values to set x and y to.</br>\
Returns: `Vector2`|
|`SetX(float x)`|
Action: Sets the x-value of the Vector2.</br>
Parameters: `float x` - the value to set x to.</br>
Returns: `void`|
|`x()`|
Action: Gets the x component.</br>
Parameters: `none`</br>
Returns: `float`|
|`SetY(float y)`|
Action: Sets the y-value of the Vector2.</br>
Parameters: `float y` - the value to set y to.</br>
Returns: `void`|
|`y()`|
Action: Gets the y component.</br>
Parameters: `none`</br>
Returns: `float`|
|`SetXY(float x, float y)`|
Action: Sets both the x and y components.</br>
Parameters: `float x, float y`</br>
Returns: `none`|

## Vector4
|Method|Details|
|:-----|-------|
|`Vector4(), Vector4(float x, float y, float z, float w)`|
Action: Constructors</br>
Parameters: `float x, float y, float z, float w` - values to set x, y, z, and w to.</br>
Returns: `Vector4`|
|`SetX(float x)`|
Action: Sets the x-value of the Vector2.</br>
Parameters: `float x` - the value to set x to.</br>
Returns: `void`|
|`x()`|
Action: Gets the x component.</br>
Parameters: `none`</br>
Returns: `float x`|
|`SetY(float y)`|
Action: Sets the y-value of the Vector2.</br>
Parameters: `float y` - the value to set y to.</br>
Returns: `void`|
|`y()`|
Action: Gets the y component.</br>
Parameters: `none`</br>
Returns: `float y`|
|`SetZ()`|
Action: Sets the z component.</br>
Parameters: </br>
Returns: `void`| 
|`z()`|
Action: Gets the z component.</br>
Parameters: </br>
Returns: `float z`|
|`SetW()`|
Action: Sets the w component.</br>
Parameters: </br>
Returns: `void`| 
|`w()`|
Action: Gets the w component.</br>
Parameters: </br>
Returns: `float w`|
|`SetXYZW()`|
Action: Sets all four components.</br>
Parameters: </br>
Returns: `void`|


## GameObject
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
|`AddRigidBody()`|Action: Adds and returns a RigidBody component to the GameObject if one doesn't already exist.</br>Parameters: `none`</br>Returns: `RigidBody*` or `nullptr`|
|`AddBoxCollider()`|Action: Adds and returns a BoxCollider component to the GameObject if one doesn't already exist.</br>Parameters: `none`</br>Returns: `BoxCollider*` or `nullptr`|
|`AddCircleCollider()`|Action: Adds and returns a CircleCollider component to the GameObject if one doesn't already exist.</br>Parameters: `none`</br>Returns: `CircleCollider*` or `nullptr`|
|`AddTileMap()`|Action: Adds and returns a TileMap component to the GameObject if one doesn't already exist.</br>Parameters: `none`</br>Returns: `TileMap*` or `nullptr`|
|`AddChild(long childID)`|Action: Adds specified child to GameObject.</br>Parameters: `long childID`</br>Returns: `void`|
|`RemoveChild()`|Action: Removes specified child from GameObject.</br>Parameters: `long childID`</br>Returns: `void`|
|`GetFirstChild()`|Action: Gets the first child found on a GameObject. Not necessarily the first in the Hierarchy.</br>Parameters: </br>Returns: |
|`HasChildren()`|Action: Returns whether the GameObject has children.</br>Parameters: `none`</br>Returns: `bool hasChildren`|
|`GetChildren()`|Action: Gets the GameObjects children.</br>Parameters: `none`</br>Returns: `std::vector<long>` - IDs of the children.|
|`HasScript()`|Action: Returns whether a GameObject Has a Script with specified name.</br>Parameters: `std::string scriptName`</br>Returns: `bool hasScript`|
|`FindChildByName(std::string childName)`|Action: Gets specified child of GameObject by name.</br>Parameters: `std::string childName`</br>Returns: `GameObject*` or `nullptr` if no child exists with that name.|

## Transform
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

## Sprite
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

## Text
|Method|Details|
|:-----|-------|
|`GetParent()`|Action: Gets the GameObject that the component is attached to.</br>Parameters: `none`</br>Returns: `GameObject*`|
|`GetParentID()`|Action: Gets the ID of the GameObject the component is attached to.</br>Parameters: `none`</br>Returns: `long ID`|
|`GetID()`|Action: Gets the ID of the component.</br>Parameters: `none`</br>Returns: `long ID`|
|`SetActive(bool isActive)`|Action: Sets the component as active or inactive.</br>Parameters: `bool isActive`</br>Returns: `void`|
|`IsActive()`|Action: Returns whether the component is active.</br>Parameters: `none`</br>Returns: `bool isActive`|
|`SetText()`|Action: Sets the text displayed by the Text component.</br>Parameters: `std::string displayedText`</br>Returns: `void`|
|`SetPivotPoint()`|Action: Sets the location the text should pivot on when rotating.</br>Parameters: `std::string pivotPoint` - string syntax expected for pivot point selection: "PivotCenter", "PivotLeft", "PivotRight", "PivotTop", "PivotBottom", "PivotTopLeft", "PivotTopRight", "PivotBottomRight","PivotBottomLeft"</br>Returns: `void`|

## Audio
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

## Button
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

## Script
|Method|Details|
|:-----|-------|
|`GetParent()`|Action: Gets the GameObject that the component is attached to.</br>Parameters: `none`</br>Returns: `GameObject*`|
|`GetParentID()`|Action: Gets the ID of the GameObject the component is attached to.</br>Parameters: `none`</br>Returns: `long ID`|
|`GetID()`|Action: Gets the ID of the component.</br>Parameters: `none`</br>Returns: `long ID`|
|`SetAttachedScript(std::string attachedScriptName)`|Action: Sets the Script that is attached to the Script component that will run during gameplay.</br>Parameters: `std::string scriptName`</br>Returns: `void`|
|`GetAttachedScript()`|Action: Gets the Script that is attached to the Script component.</br>Parameters: `none`</br>Returns: `std::string attachedScriptName`|
|`RunAwakeAndStart()`|Action: Run the Awake() and Start() Lua methods on the attached Script.</br>Parameters: `none`</br>Returns: `void`|

## Animation
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

## ScriptParameter
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

## ParameterList
|Method|Details|
|:-----|-------|
|`SetParameters(std::vector<ScriptParameter> parameters)`|Action: Sets the ScriptParameters member variable of the ParameterList.</br>Parameters: `std::vector<ScriptParameter> parameters`</br>Returns: `void`|
|`AddParameter(ScriptParameter parameters)`|Action: Add a the ScriptParameter to the ParameterList.</br>Parameters: `ScriptParameter parameter`</br>Returns: `void`|

## RigidBody
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

## Collider
|Method|Details|
|:-----|-------|
|`GetParent()`|Action: Gets the GameObject that the component is attached to.</br>Parameters: `none`</br>Returns: `GameObject*`|
|`GetParentID()`|Action: Gets the ID of the GameObject the component is attached to.</br>Parameters: `none`</br>Returns: `long ID`|
|`GetID()`|Action: Gets the ID of the component.</br>Parameters: `none`</br>Returns: `long ID`|
|`SetActive(bool isActive)`|Action: Sets the component as active or inactive.</br>Parameters: `bool isActive`</br>Returns: `none`|
|`IsActive()`|Action: Returns whether the component is active.</br>Parameters: `none`</br>Returns: `bool isActive`|

## BoxCollider - inherits from Collider
|Method|Details|
|:-----|-------|
|`GetParent()`|Action: Gets the GameObject that the component is attached to.</br>Parameters: `none`</br>Returns: `GameObject*`|
|`GetParentID()`|Action: Gets the ID of the GameObject the component is attached to.</br>Parameters: `none`</br>Returns: `long ID`|
|`GetID()`|Action: Gets the ID of the component.</br>Parameters: `none`</br>Returns: `long ID`|
|`SetActive(bool isActive)`|Action: Sets the component as active or inactive.</br>Parameters: `bool isActive`</br>Returns: `none`|
|`IsActive()`|Action: Returns whether the component is active.</br>Parameters: `none`</br>Returns: `bool isActive`|

## CircleCollider - inherits from Collider
|Method|Details|
|:-----|-------|
|`GetParent()`|Action: Gets the GameObject that the component is attached to.</br>Parameters: `none`</br>Returns: `GameObject*`|
|`GetParentID()`|Action: Gets the ID of the GameObject the component is attached to.</br>Parameters: `none`</br>Returns: `long ID`|
|`GetID()`|Action: Gets the ID of the component.</br>Parameters: `none`</br>Returns: `long ID`|
|`SetActive(bool isActive)`|Action: Sets the component as active or inactive.</br>Parameters: `bool isActive`</br>Returns: `none`|
|`IsActive()`|Action: Returns whether the component is active.</br>Parameters: `none`</br>Returns: `bool isActive`|

## CharacterController
|Method|Details|
|:-----|-------|
|`GetParent()`|Action: Gets the GameObject that the component is attached to.</br>Parameters: `none`</br>Returns: `GameObject*`|
|`GetParentID()`|Action: Gets the ID of the GameObject the component is attached to.</br>Parameters: `none`</br>Returns: `long ID`|
|`GetID()`|Action: Gets the ID of the component.</br>Parameters: `none`</br>Returns: `long ID`|
|`SetActive(bool isActive)`|Action: Sets the component as active or inactive.</br>Parameters: `bool isActive`</br>Returns: `none`|
|`IsActive()`|Action: Returns whether the component is active.</br>Parameters: `none`</br>Returns: `bool isActive`|
|`MoveToward(Vector2 direction)`|Action: Causes the GameObject to move in specified direction at the `maxSpeed` indicated inside the component settings.</br>Parameters: `Vector2 direction`</br>Returns: `void`|

## InputMapping
|Method|Details|
|:-----|-------|
|`KeyCode()`|Action: Gets the key code associated with the InputMapping.</br>Parameters: `none`</br>Returns: `std::string keyCode`|
|`InputActionName()`|Action: Gets the name of the InputAction associated with the InputMapping.</br>Parameters: `none`</br>Returns: `std::string inputActionName`|

## MappingContext
|Method|Details|
|:-----|-------|
|`Fired()`|Action: Returns true on initial press of InputAction key code.</br>Parameters: `none`</br>Returns: `bool inputActionFired`|
|`ActionPressed()`|Action: Returns true if the key code for the InputAction is currently being pressed.</br>Parameters: `none`</br>Returns: `bool inputActionPressed`|
|`GetName()`|Action: Gets the name of the MappingContext.</br>Parameters: `none`</br>Returns: `std::sting name`|
|`GetInputMappings()`|Action: Gets the InputMappings associated with the MappingContext.</br>Parameters: `none`</br>Returns: `std::vector<std::shared_ptr<InputMapping>> inputMappings`|


--------------------------------------------------------------------------------------

### Editing Engine Colors and Adding More
You may access and customize the colors of FlatEngine2D by opening and editing the file located at `engine/scripts/Colors.lua`.
You may access and customize the textures used by FlatEngine2D by opening and editing the file located at `engine/scripts/Textures.lua`.
You may access and customize the Tags used by FlatEngine2D GameObjects by opening and editing the file located at `engine/scripts/Tags.lua`.