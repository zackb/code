import bpy
import sys
import os

argv = sys.argv
if "--" in argv:
    argv = argv[argv.index("--") + 1:]
    
    if len(argv) < 2:
        print("Usage: blender --background --python blender_convert.py -- character.fbx output.glb [anim1.fbx anim2.fbx ...]")
        sys.exit(1)

    char_fbx = argv[0]
    glb_out = argv[1]
    anim_fbxs = argv[2:]

    # Clear existing objects safely
    bpy.ops.wm.read_factory_settings(use_empty=True)

    # 1. Import Character FBX with automatic_bone_orientation=False to prevent bone roll mismatches
    bpy.ops.import_scene.fbx(filepath=char_fbx, automatic_bone_orientation=False)
    
    # 2. Join all meshes SAFELY
    bpy.ops.object.select_all(action='DESELECT')
    meshes = [obj for obj in bpy.context.scene.objects if obj.type == 'MESH']
    if len(meshes) > 0:
        active_mesh = meshes[0]
        for mesh in meshes:
            if any(mod.type == 'ARMATURE' for mod in mesh.modifiers):
                active_mesh = mesh
                break
        
        for mesh in meshes:
            mesh.select_set(True)
        bpy.context.view_layer.objects.active = active_mesh
        if len(meshes) > 1:
            bpy.ops.object.join()

    main_armature = next((obj for obj in bpy.context.scene.objects if obj.type == 'ARMATURE'), None)
    if not main_armature:
        print("No armature found in character FBX!")
        sys.exit(1)
        
    main_armature.name = "Armature"
    
    if not main_armature.animation_data:
        main_armature.animation_data_create()
    
    main_armature.animation_data.action = None

    def clean_action(action):
        fcurves_to_remove = []
        
        if getattr(action, 'is_action_legacy', False) or hasattr(action, 'fcurves'):
            for fcurve in action.fcurves:
                if 'scale' in getattr(fcurve, 'data_path', '') or 'location' in getattr(fcurve, 'data_path', ''):
                    fcurves_to_remove.append((action.fcurves, fcurve))
                    
        if hasattr(action, 'layers'):
            for layer in action.layers:
                for strip in layer.strips:
                    if hasattr(strip, 'channelbags'):
                        for bag in strip.channelbags:
                            for fcurve in bag.fcurves:
                                if 'scale' in getattr(fcurve, 'data_path', '') or 'location' in getattr(fcurve, 'data_path', ''):
                                    fcurves_to_remove.append((bag.fcurves, fcurve))

        for collection, fcurve in fcurves_to_remove:
            collection.remove(fcurve)

    for action in bpy.data.actions:
        clean_action(action)

    # 3. Import each Animation FBX
    for anim_fbx in anim_fbxs:
        anim_name = os.path.splitext(os.path.basename(anim_fbx))[0]
        
        existing_objs = set(bpy.context.scene.objects)
        # MUST use automatic_bone_orientation=False here too so bone rolls perfectly match the character!
        bpy.ops.import_scene.fbx(filepath=anim_fbx, automatic_bone_orientation=False)
        new_objs = set(bpy.context.scene.objects) - existing_objs
        
        anim_armature = next((obj for obj in new_objs if obj.type == 'ARMATURE'), None)
        
        if anim_armature and anim_armature.animation_data and anim_armature.animation_data.action:
            action = anim_armature.animation_data.action
            action.name = anim_name
            
            clean_action(action)
            
            track = main_armature.animation_data.nla_tracks.new()
            track.name = anim_name
            track.strips.new(anim_name, int(action.frame_range[0]), action)
            
        # Safely remove objects using data API to avoid context errors
        for obj in new_objs:
            bpy.data.objects.remove(obj, do_unlink=True)

    # 4. Apply all transforms to bake the 90 degree X-rotation and scales
    bpy.ops.object.select_all(action='DESELECT')
    for obj in bpy.context.scene.objects:
        if obj.type in ['ARMATURE', 'MESH']:
            obj.select_set(True)
    
    # Properly bake rotation and scale together so keyframes and meshes are perfectly aligned in glTF space
    bpy.ops.object.transform_apply(location=True, rotation=True, scale=True)

    # 5. Export GLB
    bpy.ops.export_scene.gltf(
        filepath=glb_out, 
        export_format='GLB', 
        export_animations=True,
        export_nla_strips=True
    )
