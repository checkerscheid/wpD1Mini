import shutil
Import("env")

def post_program_action(source, target, env):
    src = target[0].get_abspath()
    dest = "//d1miniupdate.freakazone.com/PlatformIO/Updates/firmware.bin"
    print("dest:"+dest)
    print("source:"+src)
    shutil.copy(src, dest)

env.AddPostAction("$BUILD_DIR/firmware.bin", post_program_action)