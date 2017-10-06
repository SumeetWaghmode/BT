README: PATCHES

In the patch folder there are scripts for patches, which do not need admin rights.
The latest patch shall always be a cumulated patch including the changes since the full release
This reduces time for the user and avoids the risk that when .txt files are deleted by accidence
all changes are done within one step.

Step 1:
----------
To initialize a patch the Environment Variable 
LATEST_PATCH_RELEASE=[PATCH_RELEASE_NAME]
e.g.: set LATEST_PATCH_RELEASE=CPR_1PDM4BT_1400_UAT3

Step 2
----------
Create a subfolder [PATCH_RELEASE_NAME]

Step 3
----------
Crease a subfolder [PATCH_RELEASE_NAME]\content
Place patch content in this folder (preparation for auto build)

Step 4
----------
Crease the sript [PATCH_RELEASE_NAME].bat in the folder [PATCH_RELEASE_NAME]
Adapt the script to do the right patching
Ensure that batch scripts exit with the errorlevel0 in case of success

Step 5
----------
Crease the txt [PATCH_RELEASE_NAME].txt in the folder [PATCH_RELEASE_NAME]
Note release notes there. This file is mandatory and will be copied to the client plugin folder to indicate that the patch is already done


