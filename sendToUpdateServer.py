import shutil
Import("env")

def post_program_action(source, target, env):
	src = target[0].get_abspath()
	if env.GetProjectOption("build_with") == "1":
		build_with = "light"
	elif env.GetProjectOption("build_with") == "2":
		build_with = "io"
	elif env.GetProjectOption("build_with") == "3":
		build_with = "heating"
	else:
		build_with = ""
		
	dest = "//d1miniupdate.freakazone.com/PlatformIO/Updates/firmware" + build_with + ".bin"
	print("source:" + src)
	print("dest:" + dest)
	shutil.copy(src, dest)

env.AddPostAction("$BUILD_DIR/firmware.bin", post_program_action)