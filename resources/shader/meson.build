
# @begin header
#
# Text inside @begin/@end blocks is autogenerated by scripts/update-glsl-sources.py.
# If this file starts with an @overwrite directive, then its entire contents are managed by the script.
#
# @end header

glsl_files = files(
# @begin glsl
    'alpha_depth.frag.glsl',
    'bloom.frag.glsl',
    'boss_zoom.frag.glsl',
    'glitch.frag.glsl',
    'graph.frag.glsl',
    'hud_text.frag.glsl',
    'ingame_menu.frag.glsl',
    'laser_generic.vert.glsl',
    'marisa_laser.frag.glsl',
    'maristar_bombbg.frag.glsl',
    'masterspark.frag.glsl',
    'player_death.frag.glsl',
    'spellcard_intro.frag.glsl',
    'spellcard_outro.frag.glsl',
    'spellcard_walloftext.frag.glsl',
    'sprite_bullet.frag.glsl',
    'sprite_bullet_apple.frag.glsl',
    'sprite_circleclipped_indicator.frag.glsl',
    'sprite_circleclipped_indicator.vert.glsl',
    'sprite_default.frag.glsl',
    'sprite_default.vert.glsl',
    'sprite_filled_circle.frag.glsl',
    'sprite_filled_circle.vert.glsl',
    'sprite_negative.frag.glsl',
    'sprite_silhouette.frag.glsl',
    'sprite_silhouette.vert.glsl',
    'sprite_youmu_charged_shot.frag.glsl',
    'sprite_youmu_charged_shot.vert.glsl',
    'sprite_youmu_myon_shot.frag.glsl',
    'stage6_sky.frag.glsl',
    'stage6_sky.vert.glsl',
    'stagetext.frag.glsl',
    'standard.frag.glsl',
    'standard.vert.glsl',
    'standardnotex.frag.glsl',
    'standardnotex.vert.glsl',
    'texture_post_load.frag.glsl',
    'tower_light.frag.glsl',
    'tower_light.vert.glsl',
    'tower_wall.frag.glsl',
    'tower_wall.vert.glsl',
    'tunnel.frag.glsl',
    'youmua_bomb.frag.glsl',
    'zbuf_fog.frag.glsl',
# @end glsl
)

subdirs = [
# @begin subdirs
    'lasers',
# @end subdirs
]

glsl_targets = []

validate_glsl = false

glslc_args = [
    # '-fauto-bind-uniforms',
    # '-fauto-map-locations',

    # XXX: glslc currently doesn't support ARB_explicit_uniform_location, so we have
    # to force a standard that supports it natively to check for location overlaps.
    '-std=430core',

    # TODO: somehow shut up that annoying warning caused by forcing glsl version.
    # '-Werror',

    '--target-env=opengl',
]

glslc_depfile_args = [
    '-MD',
    '-MF', '@DEPFILE@',
]

glslc_frag_args = [
    '-fshader-stage=frag',
    '-DFRAG_STAGE',
]

glslc_vert_args = [
    '-fshader-stage=vert',
    '-DVERT_STAGE',
]

if get_option('validate_glsl') != 'false'
    glslc_command = find_program('glslc',
        required : (get_option('validate_glsl') == 'true')
    )

    if glslc_command.found()
        test_result = run_command(glslc_command,
            glslc_args,
            glslc_frag_args,
            glslc_test_file,
            '-o',
            join_paths(meson.build_root(), 'glslc-test.spv')
        )

        if test_result.returncode() == 0
            validate_glsl = true
        else
            warning(test_result.stderr())

            if get_option('validate_glsl') == 'auto'
                warning('glslc test failed, you probably have an incompatible version. GLSL validation will be disabled.')
            else
                error('glslc test failed, you probably have an incompatible version.')
            endif
        endif
    endif
endif

# @begin validate
if validate_glsl
    foreach src : glsl_files
        fname = '@0@'.format(src)
        stage = fname.split('.')[-2]

        glsl_targets += custom_target(fname.underscorify(),
            input : src,
            output : '@BASENAME@.spv',
            command : [
                glslc_command, glslc_args, glslc_depfile_args, get_variable('glslc_@0@_args'.format(stage)),
                '@INPUT@', '-o', '@OUTPUT@'
            ],
            install : false,
            build_always : false,
            build_by_default : true,
            depfile : '@0@.d'.format(fname.underscorify()),
        )
    endforeach
endif
# @end validate

foreach sd : subdirs
    subdir(sd)
endforeach
