--------------------------
-- Global Engine Colors --
--------------------------

F_Colors = {
	transparent = Vector4:new(0.0, 0.0, 0.0, 0.0),
	white = Vector4:new(1.0, 1.0, 1.0, 1.0),
	col_0 = Vector4:new(0.0, 0.0, 0.0, 1.0),
	col_1 = Vector4:new(0.1, 0.1, 0.1, 1.0),
	col_2 = Vector4:new(0.2, 0.2, 0.2, 1.0),
	col_3 = Vector4:new(0.3, 0.3, 0.3, 1.0),
	col_4 = Vector4:new(0.4, 0.4, 0.4, 1.0),
	col_5 = Vector4:new(0.5, 0.5, 0.5, 1.0),
	col_6 = Vector4:new(0.6, 0.6, 0.6, 1.0),
	col_7 = Vector4:new(0.7, 0.7, 0.7, 1.0),
	col_8 = Vector4:new(0.8, 0.8, 0.8, 1.0),
	col_9 = Vector4:new(0.9, 0.9, 0.9, 1.0),
	col_10 = Vector4:new(1.0, 1.0, 1.0, 1.0),
	red = Vector4:new(1.0, 0.0, 0.0, 1.0),
	textLight = Vector4:new(0.45, 0.45, 0.45, 1.0),
	transparentLight = Vector4:new(1.0, 1.0, 1.0, 0.05),
	----------------------
	-- Components --------
	----------------------
	componentsScrollingBg = Vector4:new(0.17, 0.17, 0.2, 1.0),
	componentSubTitleBg = Vector4:new(0.12, 0.12, 0.14, 1.0),
	componentSectionBorder = Vector4:new(0.25, 0.25, 0.27, 1.0),
	componentBorder = Vector4:new(0.2, 0.2, 0.2, 1.0),
	componentBg = Vector4:new(0.16, 0.16, 0.18, 1.0),
	pivotSelectionBg = Vector4:new(0.25, 0.25, 0.27, 1.0),
	pivotButtonHovered = Vector4:new(0.1, 0.1, 0.1, 1.0),
	openFileButtonBg = Vector4:new(0.25, 0.25, 0.26, 1.0),
	openFileButtonHovered = Vector4:new(0.3, 0.3, 0.32, 1.0),

	shapesScrollingBg = Vector4:new(0.25, 0.34, 0.56, 0.4),
	shapeBg = Vector4:new(0.16, 0.16, 0.18, 1.0),
	jointsScrollingBg = Vector4:new(0.25, 0.34, 0.56, 0.4),
	jointBg = Vector4:new(0.16, 0.16, 0.18, 1.0),
	-- Colliders --
	sensorActive = Vector4:new(1.0, 1.0, 0.27, 1.0),
	sensorInactive = Vector4:new(1.0, 0.0, 0.0, 1.0),
	sensorActiveLight =  Vector4:new(1.0, 1.0, 0.27, 0.25),
	sensorInactiveLight =  Vector4:new(1.0, 0.0, 0.0, 0.25),
	colliderActive = Vector4:new(0.19, 0.9, 0.2, 1.0),
	colliderActiveLight = Vector4:new(0.19, 0.9, 0.2, 0.25),
	colliderInactive = Vector4:new(0.9, 0.2, 0.2, 1.0),
	colliderInactiveLight = Vector4:new(0.9, 0.2, 0.2, 0.25),
	-- Box
	boxColliderActive = Vector4:new(0.19, 0.9, 0.2, 1.0),	
	boxColliderActiveLight = Vector4:new(0.19, 0.9, 0.2, 0.3),
	boxColliderInactive = Vector4:new(0.9, 0.2, 0.2, 1.0),	
	boxColliderInactiveLight = Vector4:new(0.9, 0.2, 0.2, 0.25),	
	-- Circle	
	circleColliderActive = Vector4:new(0.19, 0.9, 0.2, 1.0),
	circleColliderActiveLight = Vector4:new(0.19, 0.9, 0.2, 0.25),
	circleColliderInactive = Vector4:new(0.9, 0.2, 0.2, 1.0),
	circleColliderInactiveLight = Vector4:new(0.9, 0.2, 0.2, 0.25),
	-- Capsule
	capsuleColliderActive = Vector4:new(0.19, 0.9, 0.2, 1.0),
	capsuleColliderInactive = Vector4:new(0.9, 0.2, 0.2, 1.0),
	capsuleColliderActiveLight = Vector4:new(0.19, 0.9, 0.2, 0.25),
	capsuleColliderInactiveLight = Vector4:new(0.9, 0.2, 0.2, 0.25),
	-- Polygon
	polygonColliderActive = Vector4:new(0.19, 0.9, 0.2, 1.0),
	polygonColliderActiveLight = Vector4:new(0.19, 0.9, 0.2, 0.25),
	polygonColliderInactive = Vector4:new(1.0, 0.0, 0.0, 1.0),
	polygonColliderInactiveLight = Vector4:new(1.0, 0.0, 0.0, 0.25),
	-- Chain
	chainColliderActive = Vector4:new(0.19, 0.9, 0.2, 1.0),
	chainColliderActiveLight = Vector4:new(0.19, 0.9, 0.2, 0.25),
	chainColliderInactive = Vector4:new(1.0, 0.0, 0.0, 1.0),
	chainColliderInactiveLight = Vector4:new(1.0, 0.0, 0.0, 0.25),
	chainColliderEndSegmentsActive = Vector4:new(1.0, 0.0, 0.0, 1.0),
	chainColliderEndSegmentsActiveLight = Vector4:new(1.0, 0.0, 0.0, 0.25),

	rayCast = Vector4:new(0.8, 0.75, 0.05, 1.0),
	-- Button Components
	buttonComponentActive = Vector4:new(0.75, 0.06, 0.41, 1.0),
	buttonComponentInactive = Vector4:new(0.75, 0.06, 0.41, 0.4),
	buttonComponentHovered = Vector4:new(0.42, 0.75, 0.06, 1.0),
	-- Canvas
	canvasBox = Vector4:new(0.76, 0.42, 0.0, 0.5),
	-- Camera
	cameraBox = Vector4:new(1.0, 0.11, 0.11, 0.27),
	----------------------
	-- Windows -----------
	----------------------
	-- Main
	paneBg = Vector4:new(0.15, 0.15, 0.16, 1.0),
	windowBg = Vector4:new(0.0, 0.0, 0.0, 1.0),
	viewportTitleBg = Vector4:new(0.06, 0.06, 0.07, 1.0),
	viewportTitleBgActive = Vector4:new(0.09, 0.09, 0.1, 1.0),
	outerWindow = Vector4:new(0.13, 0.13, 0.15, 1.0),
	innerWindow = Vector4:new(0.1, 0.1, 0.12, 1.0),
	singleItem = Vector4:new(0.16, 0.16, 0.17, 1.0),
	singleItemDark = Vector4:new(0.09, 0.09, 0.13, 1.0),
	windowTitleBg = Vector4:new(0.25, 0.25, 0.25, 1.0),

	sectionHeaderBg = Vector4:new(0.1, 0.1, 0.12, 1.0),
	sectionHeaderBorder = Vector4:new(0.25, 0.25, 0.25, 1.0),
	-- Scrollbar
	scrollbarBg = Vector4:new(0.22, 0.22, 0.24, 1.0),
	scrollbarGrab = Vector4:new(0.42, 0.42, 0.45, 1.0),
	scrollbarGrabActive = Vector4:new(0.45, 0.45, 0.47, 1.0),
	scrollbarGrabHovered = Vector4:new(0.55, 0.55, 0.58, 1.0),
	--------------------
	--- Scene View ---
	--------------------
	-- Buttons container
	cursorModeSelectBg = Vector4:new(0.9, 0.9, 0.9, 0.05),
	cursorModeSelectBorder = Vector4:new(0.9, 0.9, 0.9, 0.2),
	-- Buttons
	cursorModeButtonHover = Vector4:new(0.9, 0.9, 0.9, 0.3),
	cursorModeButtonHoverSelected = Vector4:new(0.4, 0.4, 0.4, 0.6),
	cursorModeButtonSelectedBorder = Vector4:new(1, 1, 1, 1);
	selectedCursorModeButtonBg =  Vector4:new(0.4, 0.4, 0.4, 0.8),
	-- Modals
	modalWindowDimBg = Vector4:new(0.1, 0.1, 0.1, 1),
	-- Project Hub
	selectProjectBg = Vector4:new(0.08, 0.08, 0.10, 1.0),
	projectHubButton = Vector4:new(0.3, 0.3, 0.55, 1.0),
	projectHubButtonActive = Vector4:new(0.2, 0.2, 0.45, 1.0),
	projectHubButtonHovered = Vector4:new(0.4, 0.4, 0.65, 1.0),
	projectSelectionTable = Vector4:new(0.16, 0.16, 0.17, 1.0),
	projectHubTableOutline = Vector4:new(0.36, 0.36, 0.37, 1.0),
	-- Log
	logText = Vector4:new(0.75, 0.75, 0.75, 1.0),
	logBg =Vector4:new(0.23, 0.23, 0.26, 1.0),
	logOutline = Vector4:new(0.25, 0.25, 0.27, 1.0),
	-- File Explorer
	explorerFilesPanelBg =  Vector4:new(0.2, 0.2, 0.23, 1.0),
	panelTitleBg =  Vector4:new(0.14, 0.14, 0.14, 1.0),
	filePanelTopBar = Vector4:new(0.2, 0.2, 0.22, 0.0),
	filesTopBarButtonBg = Vector4:new(0.25, 0.25, 0.27, 1.0),
	filesTopBarButtonHover = Vector4:new(0.5, 0.5, 0.54, 1.0),
	filesPanelOutline = Vector4:new(0.25, 0.25, 0.27, 1.0),
	selectedFileBg =  Vector4:new(0.75, 0.75, 0.75, 0.11),
	selectedFileOutline =  Vector4:new(0.75, 0.75, 0.75, 0.45),
	normalFileBg =  Vector4:new(0.75, 0.75, 0.75, 0.03),
	normalFileOutline =  Vector4:new(0.75, 0.75, 0.75, 0.15),
	hoveredFileBg =  Vector4:new(0.75, 0.75, 0.75, 0.05),
	hoveredFileOutline =  Vector4:new(0.75, 0.75, 0.75, 0.05),
	-- TileSet Editing
	tileSetTileBorder = Vector4:new(0.92, 1.0, 0.0, 0.5),
	tileSetHoveredTile = Vector4:new(1, 1, 1, 0.35),
	tileSetHoveredTileBorder = Vector4:new(1, 1, 1, 1),
	tileSetSelectedTile = Vector4:new(1, 1, 1, 0.15),
	tileSetHoldingTile = Vector4:new(1, 1, 1, 0.45),
	-- TileMap
	tileMapBoxFocused = Vector4:new(0.71, 0.0, 0.88, 0.8),
	tileMapGridBgFocused = Vector4:new(0.71, 0.0, 0.88, 0.1),
	tileMapBoxUnfocused = Vector4:new(0.71, 0.0, 0.88, 0.4),
	tileMapGridBgUnfocused = Vector4:new(0.71, 0.0, 0.88, 0.05),
	tileMapGridLines = Vector4:new(0.71, 0.0, 0.88, 0.15),
	tileBoxColHighlight = Vector4:new(0.2, 1, 0.2, 0.4),
	tileBoxColHoveredHighlight = Vector4:new(0.2, 1, 0.2, 0.4),
	tileMultiSelectHighlightDragging = Vector4:new(1, 1, 1, 0.35),
	tileMultiSelectHighlight = Vector4:new(1, 1, 1, 0.15),
	tileMultiSelectHighlightBorder = Vector4:new(1, 1, 1, 0.95),
	tileMoveModeClickBorder = Vector4:new(1, 1, 1, 0.95),
	tileSetIconHovered = Vector4:new(0.8, 0.8, 0.8, 0.8),
	tileSetEraseModeClick = Vector4:new(0.5, 0, 0, 0.6);
	tileSetEraseModeClickBorder = Vector4:new(0.8, 0, 0, 1);
	tileSetEraseModeHover = Vector4:new(1, 0, 0, 0.5);
	tileSetEraseModeHoverBorder = Vector4:new(1, 0.1, 0.1, 1);
	tileSelectModeClickBorder = Vector4:new(1, 1, 1, 1),
	-- Animator
	timelineGridBg = Vector4:new(0.2, 0.2, 0.2, 1),
	timelineGridlinesDark = Vector4:new(0.2, 0.2, 0.2, 1),
	-- Docking
	dockingPreview = Vector4:new(0.3, 0.3, 0.65, 1.0),
	dockingPreviewEmpty = Vector4:new(0.3, 0.3, 0.65, 1.0),
	-- ImGui Key Colors
	frameBg = Vector4:new(0.2, 0.2, 0.2, 1.0),
	frameBgActive = Vector4:new(0.2, 0.2, 0.2, 1.0),
	frameBgHovered = Vector4:new(0.2, 0.2, 0.2, 1.0),
	-- tabs
	tab = Vector4:new(0.15, 0.15, 0.2, 1.0),
	tabActive = Vector4:new(0.4, 0.4, 0.42, 1.0),
	tabHovered = Vector4:new(0.4, 0.4, 0.42, 1.0),
	tabUnfocused = Vector4:new(0.1, 0.1, 0.1, 1.0),
	tabUnfocusedActive = Vector4:new(0.3, 0.3, 0.31, 1.0),
	-- titles
	titleBg = Vector4:new(0.0, 0.0, 0.0, 1.0),
	titleBgActive = Vector4:new(0.2, 0.2, 0.25, 1.0),
	titleBgCollapsed = Vector4:new(0.2, 0.2, 0.2, 1.0),
	-- Text
	textSelectedBg = Vector4:new(0.2, 0.2, 0.2, 1.0),
	-- Resizers
	resizeGrip = Vector4:new(0.3, 0.3, 0.65, 1.0),
	resizeGripHovered = Vector4:new(0.35, 0.35, 0.75, 1.0),
	resizeGripActive = Vector4:new(0.2, 0.2, 0.5, 0.8),
	-- Misc (not sure what they're or)
	popupBg = Vector4:new(0.2, 0.2, 0.2, 1.0),
	navWindowHighlight = Vector4:new(0.2, 0.2, 0.2, 1.0),
	navHighlight = Vector4:new(0.2, 0.2, 0.2, 1.0),
	navWindowDimBg = Vector4:new(0.2, 0.2, 0.2, 1.0),
	modalWindowDimBg = Vector4:new(0.2, 0.2, 0.2, 1.0),
	dropTarget = Vector4:new(1.0, 0.95, 0.0, 1.0),
	---------------------
	--- Custom Colors ---
	---------------------
	-- Inputs
	input = Vector4:new(0.28, 0.28, 0.3, 1.0),
	-- Tables
	noEditTableText = Vector4:new(0.75, 0.75, 0.81, 1.0),
	noEditTableRowFieldBg = Vector4:new(0.1, 0.1, 0.5, 0.2),
	noEditTableRowValueBg = Vector4:new(0.1, 0.1, 0.5, 0.2),
	tableCellLight = Vector4:new(0.19, 0.19, 0.21, 1.0),
	tableCellDark = Vector4:new(0.24, 0.24, 0.27, 1.0),
	fileExplorerTableRowBg = Vector4:new(0.18, 0.18, 0.208, 1.0),
	fileExplorerTableRowBgAlt = Vector4:new(0.2, 0.2, 0.23, 1.0),
	-- Trees
	treeSelectable = Vector4:new(0.15, 0.15, 0.15, 0.7),
	treeSelectableHovered = Vector4:new(0.3, 0.35, 0.65, 0.7),
	treeSelectableActive = Vector4:new(0.2, 0.25, 0.45, 0.7),
	treeSelectableSelected = Vector4:new(0.5, 0.5, 0.8, 0.7),
	hierarchyChildObject = Vector4:new(0.3, 0.3, 0.7, 0.2),
	-- Combos
	comboBg = Vector4:new(0.19, 0.19, 0.21, 1.0),
	comboHovered = Vector4:new(0.25, 0.25, 0.26, 1.0),
	comboSelectable = Vector4:new(0.34, .34, .4, 1.0),
	comboSelected = Vector4:new(0.45, 0.45, 0.50, 1.0),
	comboHighlighted = Vector4:new(0.25, 0.25, 0.28, 1.0),
	comboArrow = Vector4:new(0.11, 0.11, 0.13, 1.0),
	comboArrowHovered = Vector4:new(0.15, 0.15, 0.16, 1.0),
	-- Buttons
	button = Vector4:new(0.3, 0.3, 0.65, 1.0),
	buttonHovered = Vector4:new(0.35, 0.35, 0.75, 1.0),
	buttonActive = Vector4:new(0.2, 0.2, 0.5, 0.8),
	imageButton = Vector4:new(0.18, 0.18, 0.2, 1.0),
	imageButtonDark = Vector4:new(0.15, 0.15, 0.15, 1.0),
	imageButtonHovered = Vector4:new(0.3, 0.3, 0.3, 1.0),
	imageButtonActive = Vector4:new(0.1, 0.1, 0.1, 1.0),
	imageButtonTint = Vector4:new(1.0, 1.0, 1.0, 1.0),
	-- Sliders/Drags
	sliderBg = Vector4:new(0.11, 0.11, 0.11, 1.0),
	sliderHovered =  Vector4:new(0.10, 0.10, 0.10, 1.0),
	sliderActive = Vector4:new(0.3, 0.3, 0.32, 1.0),
	sliderGrab = Vector4:new(0.3, 0.3, 0.65, 1.0),
	sliderGrabActive = Vector4:new(0.09, 0.09, 0.13, 1.0),
	sliderGrabHovered = Vector4:new(0.35, 0.35, 0.75, 1.0),
	drag = Vector4:new(0.2, 0.2, 0.2, 0.0),
	dragHovered = Vector4:new(0.45, 0.45, 0.45, 1.0),	
	dragActive = Vector4:new(0.45, 0.45, 0.45, 1.0),	
	-- Checkboxes
	checkboxBg = Vector4:new(0.28, 0.28, 0.29, 1.0),
	checkboxCheck = Vector4:new(0.45, 0.45, 0.9, 1.0),
	checkboxHovered = Vector4:new(0.31, 0.31, 0.32, 1.0),
	checkboxActive = Vector4:new(0.15, 0.15, 0.23, 1.0),
}