# FlatEngine - 2D Game Engine


![ReleaseBuildV1](https://github.com/user-attachments/assets/e7a38d3e-963c-4bcb-a5f1-a01b33d6dda3)

## UPDATE

There is now a published release for FlatEngine2D!  Please see the "Releases" section on the right-hand side for a download link and read up on the ReadMe for details on how to use FlatEngine.

### Disclaimer

This project is in active development.

This is a hobby project.
It will likely crash eventually so save often. THERE IS NO AUTOSAVE.
Things will change, sometimes drastically, with updates.
Your results and the usefulness of FlatEngine may vary.
Key systems are not optimized.

Many things in this README are already out of date so keep that in mind when reading.  The general architecture of FlatEngine won't change that much over the course of development.


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

Lua/Sol is the scripting language embedded in FlatEngine. Scripts are contained in files with the extension ".scp.lua" in order to be controlled as needed within the engine.

See "Using FlatEngine" below for a detailed walkthrough of how to use Lua in FlatEngine.


--------------------------------------------------------------------------------------


## Engine Components

### Tags

Tags are a list of properties that a GameObject can have that can be queried using 

`GameObject::HasTag("tagName")` 

The Tags system is also used in the collision detection system to prevent objects that should not interact from interacting, based on the tags each GameObject is set to Ignore.  I would eventually like to extend this system to use a Lua script file for the available tags, that way they are editable by the user, but for now they are hard-coded into the engine.

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
12. CircleCollider (work in progress)
13. CompositeCollider (work in progress)
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

**EDIT: This is outdated.**
Scripts in FlatEngine are written in Lua in ".scp.lua" files.  The Script component is merely a container for a script reference.  Eventually I plan on enabling exposure of Lua variables inside the Script component but that is not currently a feature of FlatEngine.  See the explanation farther down on how scripting works in FlatEngine for more details.

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

Animation components are one of the most powerful components that can be attached to a GameObject.  They currently support animation of Transform, Sprite, Text, and Audio components.  Additionally, you may choose to add Events to each Animation.  Animation Events allow you to call Lua functions at a particular keyframe and pass up to 5 parameters to it, (string, int, long, float, double, bool, or Vector2).  These are extremely powerful and useful to take advantage of.  I plan to add more components to the list of components you can animate, but I believe this is a very versitile grouping and likely you are able to do most of what you need to with them.  For more details on the Animation component, please refer to the tour video I will be posted in the ReadMe.  I will update this with more detailed information in the future.

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
|Is Solid?|Determines whether other collisiders should pass through this collider or if it is just a trigger.|
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
`OnButtonMouseOver()` -- Called every frame the mouse is hovering a Button
`OnButtonMouseEnter()` -- Called on the first frame a mouse is hovering a Button
`OnButtonMouseLeave()` -- Called when the mouse stops hovering a Button
`OnButtonLeftClick()` -- Called when a Button is left clicked
`OnButtonRightClick()` -- Called when a Button is right clicked
Any Animation Event function that is called during an Animation



--------------------------------------------------------------------------------------



## Lua Functions Available

This is list of all functions that can be called using Lua to interact with the engine and with GameObjects and components:


`GetObjectByName(std::string objectName)`<br/>
Action: Gets a GameObject in the loaded scene by name.<br/>
Parameters: objectName - name of the object you want.<br/>
Returns: GameObject*<br/>

`LoadScene(std::string sceneName)`<br/>
Action: Loads a scene<br/>
Parameters: sceneName - name of the scene to load.<br/>
Returns: void<br/>

`LogString(std::string line)`<br/>
Action: Log a string to the console.<br/>
Parameters: line - string to log<br/>
Returns: void<br/>

`LogInt(int value, std::string line = "")`<br/>
Action: Log an int to the console and a string that will be prefixed to the value.<br/>
Parameters: value - value to log, line - (optional) string that will be prefixed to the value<br/>
Returns: void<br/>

`LogFloat(float value, std::string line = "")`<br/>
Action: Log an int to the console and a string that will be prefixed to the value.<br/>
Parameters: value - value to log, line - (optional) string that will be prefixed to the value<br/>
Returns: void<br/>

`LogDouble(double value, std::string line = "")`<br/>
Action: Log an int to the console and a string that will be prefixed to the value.<br/>
Parameters: value - value to log, line - (optional) string that will be prefixed to the value<br/>
Returns: void<br/>

`LogLong(long value, std::string line = "")`<br/>
Action: Log an int to the console and a string that will be prefixed to the value.<br/>
Parameters: value - value to log, line - (optional) string that will be prefixed to the value<br/>
Returns: void<br/>

`GetMappingContext(std::string contextName)`<br/>
Action: Get a copy of a Mapping Context object by name.<br/>
Parameters: contextName - name of the Mapping Context<br/>
Returns: MappingContext*<br/>

`Instantiate(std::string prefabName, Vector2 position)`<br/>
Action: Instantiate a Prefab at a specific location<br/>
Parameters: prefabName - name of the Prefab to spawn, position - the position in the game world to spawn the Prefab.<br/>
Returns: GameObject*<br/>

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

`RandomNumber(unsigned int min, unsigned int max)`<br/>
Action: Generates a random number within specified range.<br/>
Parameters: min - the lowest value (inclusive), max - the highest value (inclusive)<br/>
Returns: int<br/>

# UNDER CONSTRUCTION

### FlatEngine classes exposed to Lua and their methods (Lua usertypes)

## Vector2

|Method|Details|
|:-----|-------|
|Vector2(), Vector2(float x,float y)|Action: Constructors</br>Parameters: x,y,z,w - values to set x and y to.</br>Returns: Vector2
|SetX(float x)|Action: Sets the x-value of the Vector2.</br>Parameters: x - the value to set x to.</br>Returns: void|
|x|Action: </br>Parameters: </br>Returns: |
|SetY(float y)|Action: Sets the y-value of the Vector2.</br>Parameters: y - the value to set x to.</br>Returns: void|
|y|Action: </br>Parameters: </br>Returns: |
|SetXY(float x, float y)|Action: </br>Parameters: </br>Returns: |

## Vector4
|Method|Details|
|:-----|-------|
|Vector4(), Vector4(float x, float y, float z, float w)|Action: Constructors</br></br>Parameters: x,y,z,w - values to set x, y, z, and w to.</br>Returns: Vector4
|SetX|Action: </br>Parameters: </br>Returns: |
|x|Action: </br>Parameters: </br>Returns: |
|SetY|Action: </br>Parameters: </br>Returns: | 
|y|Action: </br>Parameters: </br>Returns: |
|SetZ|Action: </br>Parameters: </br>Returns: | 
|z|Action: </br>Parameters: </br>Returns: |
|SetW|Action: </br>Parameters: </br>Returns: | 
|w|Action: </br>Parameters: </br>Returns: |
|SetXYZW|Action: </br>Parameters: </br>Returns: |


## GameObject
|Method|Details|
|:-----|-------|
|GetID|Action: </br>Parameters: </br>Returns: |
|GetName|Action: </br>Parameters: </br>Returns: |
|SetName|Action: </br>Parameters: </br>Returns: |
|IsActive|Action: </br>Parameters: </br>Returns: |
|SetActive|Action: </br>Parameters: </br>Returns: |
|GetParent|Action: </br>Parameters: </br>Returns: |
|GetParentID|Action: </br>Parameters: </br>Returns: |
|HasTag|Action: </br>Parameters: </br>Returns: |
|GetTransform|Action: </br>Parameters: </br>Returns: |
|AddSprite|Action: </br>Parameters: </br>Returns: |
|GetSprite|Action: </br>Parameters: </br>Returns: |
|GetCamera|Action: </br>Parameters: </br>Returns: |
|GetAnimation|Action: </br>Parameters: </br>Returns: |
|GetAudio|Action: </br>Parameters: </br>Returns: |
|GetButton|Action: </br>Parameters: </br>Returns: |
|GetCanvas|Action: </br>Parameters: </br>Returns: |
|GetText|Action: </br>Parameters: </br>Returns: |
|GetCharacterController|Action: </br>Parameters: </br>Returns: |
|GetRigidBody|Action: </br>Parameters: </br>Returns: |
|GetBoxColliders|Action: </br>Parameters: </br>Returns: |
|GetBoxCollider|Action: </br>Parameters: </br>Returns: |
|GetFirstChild|Action: </br>Parameters: </br>Returns: |
|HasChildren|Action: </br>Parameters: </br>Returns: |
|GetChildren|Action: </br>Parameters: </br>Returns: |
|HasScript|Action: </br>Parameters: </br>Returns: |
|FindChildByName|Action: </br>Parameters: </br>Returns: |

## Transform
|Method|Details|
|:-----|-------|
|GetParent|Action: </br>Parameters: </br>Returns: |
|GetParentID|Action: </br>Parameters: </br>Returns: |
|GetID|Action: </br>Parameters: </br>Returns: |
|SetPosition|Action: </br>Parameters: </br>Returns: |
|GetPosition|Action: </br>Parameters: </br>Returns: |
|GetTruePosition|Action: </br>Parameters: </br>Returns: |
|SetRotation|Action: </br>Parameters: </br>Returns: |
|GetRotation|Action: </br>Parameters: </br>Returns: |
|SetScale|Action: </br>Parameters: </br>Returns: |
|GetScale|Action: </br>Parameters: </br>Returns: |
|LookAt|Action: </br>Parameters: </br>Returns: |

## Sprite
|Method|Details|
|:-----|-------|
|SetActive|Action: </br>Parameters: </br>Returns: |
|IsActive|Action: </br>Parameters: </br>Returns: |
|GetParent|Action: </br>Parameters: </br>Returns: |
|GetParentID|Action: </br>Parameters: </br>Returns: |
|GetID|Action: </br>Parameters: </br>Returns: |
|SetTexture|Action: </br>Parameters: </br>Returns: |
|GetPath|Action: </br>Parameters: </br>Returns: |
|SetScale|Action: </br>Parameters: </br>Returns: |
|GetScale|Action: </br>Parameters: </br>Returns: |
|GetTextureWidth|Action: </br>Parameters: </br>Returns: |
|GetTextureHeight|Action: </br>Parameters: </br>Returns: |
|SetTintColor|Action: </br>Parameters: </br>Returns: |
|GetTintColor|Action: </br>Parameters: </br>Returns: |

## Audio
|Method|Details|
|:-----|-------|
|GetParent|Action: </br>Parameters: </br>Returns: |
|GetParentID|Action: </br>Parameters: </br>Returns: |
|SetActive|Action: </br>Parameters: </br>Returns: |
|IsActive|Action: </br>Parameters: </br>Returns: |
|GetID|Action: </br>Parameters: </br>Returns: |
|IsMusicPlaying|Action: </br>Parameters: </br>Returns: |
|Play|Action: </br>Parameters: </br>Returns: |
|Pause|Action: </br>Parameters: </br>Returns: |
|Stop|Action: </br>Parameters: </br>Returns: |
|StopAll|Action: </br>Parameters: </br>Returns: |

## Animation
|Method|Details|
|:-----|-------|
|GetParent|Action: </br>Parameters: </br>Returns: |
|GetParentID|Action: </br>Parameters: </br>Returns: |
|SetActive|Action: </br>Parameters: </br>Returns: |
|IsActive|Action: </br>Parameters: </br>Returns: |
|GetID|Action: </br>Parameters: </br>Returns: |
|Play|Action: </br>Parameters: </br>Returns: |
|Stop|Action: </br>Parameters: </br>Returns: |
|StopAll|Action: </br>Parameters: </br>Returns: |
|IsPlaying|Action: </br>Parameters: </br>Returns: |
|HasAnimation|Action: </br>Parameters: </br>Returns: |

## EventParameter
|Method|Details|
|:-----|-------|
|type|Action: </br>Parameters: </br>Returns: |
|string|Action: </br>Parameters: </br>Returns: |
|int|Action: </br>Parameters: </br>Returns: |
|long|Action: </br>Parameters: </br>Returns: |
|float|Action: </br>Parameters: </br>Returns: |
|double|Action: </br>Parameters: </br>Returns: |
|bool|Action: </br>Parameters: </br>Returns: |
|Vector2|Action: </br>Parameters: </br>Returns: |

## RigidBody
|Method|Details|
|:-----|-------|
|SetActive|Action: </br>Parameters: </br>Returns: |
|IsActive|Action: </br>Parameters: </br>Returns: |
|GetParent|Action: </br>Parameters: </br>Returns: |
|GetParentID|Action: </br>Parameters: </br>Returns: |
|GetID|Action: </br>Parameters: </br>Returns: |
|SetMass|Action: </br>Parameters: </br>Returns: |
|GetMass|Action: </br>Parameters: </br>Returns: |
|SetGravity|Action: </br>Parameters: </br>Returns: |
|GetGravity|Action: </br>Parameters: </br>Returns: |
|SetFallingGravity|Action: </br>Parameters: </br>Returns: |
|GetFallingGravity|Action: </br>Parameters: </br>Returns: |
|SetFriction|Action: </br>Parameters: </br>Returns: |
|GetFriction|Action: </br>Parameters: </br>Returns: |
|SetAngularDrag|Action: </br>Parameters: </br>Returns: |
|GetAngularDrag|Action: </br>Parameters: </br>Returns: |
|SetAngularVelocity|Action: </br>Parameters: </br>Returns: |
|GetAngularVelocity|Action: </br>Parameters: </br>Returns: |
|SetTorquesAllowed|Action: </br>Parameters: </br>Returns: |
|TorquesAllowed|Action: </br>Parameters: </br>Returns: |
|AddForce|Action: </br>Parameters: </br>Returns: |
|AddTorque|Action: </br>Parameters: </br>Returns: |
|GetVelocity|Action: </br>Parameters: </br>Returns: |
|SetPendingForces|Action: </br>Parameters: </br>Returns: |
|GetPendingForces|Action: </br>Parameters: </br>Returns: |
|SetTerminalVelocity|Action: </br>Parameters: </br>Returns: |
|GetTerminalVelocity|Action: </br>Parameters: </br>Returns: |


## BoxCollider
|Method|Details|
|:-----|-------|
|GetParent|Action: </br>Parameters: </br>Returns: |
|GetParentID|Action: </br>Parameters: </br>Returns: |
|SetActive|Action: </br>Parameters: </br>Returns: |
|IsActive|Action: </br>Parameters: </br>Returns: |
|GetID|Action: </br>Parameters: </br>Returns: |


## CharacterController
|Method|Details|
|:-----|-------|
|MoveToward|Action: </br>Parameters: </br>Returns: |
|GetParent|Action: </br>Parameters: </br>Returns: |
|GetParentID|Action: </br>Parameters: </br>Returns: |
|SetActive|Action: </br>Parameters: </br>Returns: |
|IsActive|Action: </br>Parameters: </br>Returns: |
|GetID|Action: </br>Parameters: </br>Returns: |


## MappingContext
|Method|Details|
|:-----|-------|
|Fired|Action: </br>Parameters: </br>Returns: |
|ActionPressed|Action: </br>Parameters: </br>Returns: |
|GetName|Action: </br>Parameters: </br>Returns: |




--------------------------------------------------------------------------------------

### Editing Engine Colors and Adding More



### Editing Engine Textures / Icons
