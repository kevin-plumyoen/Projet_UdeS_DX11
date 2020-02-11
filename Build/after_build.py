import sys
import os
import glob
import shutil
import errno
import stat
import itertools

def silent_remove(filename):
	try:
		os.remove(filename)
	except OSError as e:
		if e.errno != errno.ENOENT:
			raise
			
def silent_remove_tree(filename):
	try:
		shutil.rmtree(filename, True)
	except OSError as e:
		if e.errno != errno.ENOENT:
			raise

def install_file(source_file, dest_file):
	print("  installing '" + source_file + "' to '" + dest_file + "'")
	silent_remove(dest_file)
	shutil.copy(source_file, dest_file)
	os.chmod(dest_file, 0o666)

def install_dir(source_dir, dest_dir):
	print("  installing '" + source_dir + "' to '" + dest_dir + "'")
	silent_remove_tree(dest_dir)
	shutil.copytree(source_dir, dest_dir)
	os.chmod(dest_dir, 0o666)
	for root, dirs, files in os.walk(dest_dir):
		for momo in dirs:
			os.chmod(os.path.join(root, momo), 0o666)
		for momo in files:
			os.chmod(os.path.join(root, momo), 0o666)
	
def get_shaders_root(project_dir):
	return os.path.join(project_dir, "Sources", "Shader")

def get_resources_root(project_dir):
	return os.path.join(project_dir, "Resources")
	
def get_physx_root(project_dir):
	return os.path.join(project_dir, "lib", "PhysX")
	
def get_assimp_root(project_dir):
	return os.path.join(project_dir, "lib", "assimp")
	
def get_pxshared_dll_names(configuration, platform):
	if platform == "x64":
		if configuration in ["Release", "Profile", "Gold"]:
			return ["PxFoundation_x64.dll", "PxPvdSDK_x64.dll"]
		elif configuration == "Debug":
			return ["PxFoundationDEBUG_x64.dll", "PxPvdSDKDEBUG_x64.dll"]
		else:
			raise NotImplementedError
	else:
		raise NotImplementedError

def get_physx_dll_names(configuration, platform):
	if platform == "x64":
		if configuration in ["Release", "Profile", "Gold"]:
			return ["PhysX3_x64.dll", "PhysX3Common_x64.dll","PhysX3Cooking_x64.dll", "nvToolsExt64_1.dll"]
		elif configuration == "Debug":
			return ["PhysX3DEBUG_x64.dll", "PhysX3CommonDEBUG_x64.dll","PhysX3CookingDEBUG_x64.dll", "nvToolsExt64_1.dll"]
		else:
			raise NotImplementedError
	else:
		raise NotImplementedError

def install_physx_dlls(physx_root, out_dir, configuration, platform):
	print("installing PhysX dlls...")
	pxshared_dlls = map(lambda dll_name: os.path.join(physx_root, "PxShared", "Bin", dll_name) , get_pxshared_dll_names(configuration, platform))
	pxphysx_dlls = map(lambda dll_name: os.path.join(physx_root, "PhysX-3.4", "Bin", dll_name) , get_physx_dll_names(configuration, platform))
	dlls_to_install = itertools.chain(pxshared_dlls, pxphysx_dlls)

	for dll_to_install in dlls_to_install:
		basename = os.path.basename(dll_to_install)
		destination_file = os.path.join(out_dir, basename)
		install_file(dll_to_install, destination_file)
		
def install_assimp_dlls(assimp_root, out_dir, configuration, platform):
	print("installing assimp dlls...")

	assimp_configuration_folder = ""

	if configuration in [ "Profile", "Release", "Gold" ]:
		assimp_configuration_folder = "Release"
	else:
		assimp_configuration_folder = "Debug"

	dll_to_install= os.path.join(assimp_root, "bin", assimp_configuration_folder,"assimp-vc140-mt.dll")
	basename = os.path.basename(dll_to_install)
	destination_file = os.path.join(out_dir, basename)
	install_file(dll_to_install, destination_file)
	
def install_resources(resources_root, out_dir):
	print("installing resources...")
	ressources_to_install = glob.glob(resources_root + "\\*")
	for resource_to_install in ressources_to_install:
		if os.path.isdir(resource_to_install):
			basename = os.path.basename(resource_to_install)
			destination_dir = os.path.join(out_dir, basename)
			
			install_dir(resource_to_install, destination_dir)
		else:
			basename = os.path.basename(resource_to_install)
			destination_file = os.path.join(out_dir, basename)
			
			install_file(resource_to_install, destination_file)

def install_shaders(shaders_root, out_dir):
	print("installing shaders...")
	shaders_to_install = glob.glob(shaders_root + "\\*")
	for shader_to_install in shaders_to_install:
		basename = os.path.basename(shader_to_install)
		destination_file = os.path.join(out_dir, basename)
		
		install_file(shader_to_install, destination_file)
	
def execute_after_build(project_dir, out_dir, configuration, platform):
	install_resources(get_resources_root(project_dir), out_dir)
	install_shaders(get_shaders_root(project_dir), out_dir)
	install_physx_dlls(get_physx_root(project_dir), out_dir, configuration, platform)
	install_assimp_dlls(get_assimp_root(project_dir), out_dir, configuration, platform)

if __name__ == "__main__":
	execute_after_build(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4])