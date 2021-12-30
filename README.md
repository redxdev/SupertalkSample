# Supertalk Sample

Small sample project for Supertalk. You must download the latest version of the plugin at https://github.com/redxdev/Supertalk and place it in Plugins.

This is only setup to work with UE5 but should work with UE4 with minor modifications to this project + Supertalk itself.

This is a *very* simple sample project that doesn't implement any UI or anything - it simply prints out lines from the script via
AddOnScreenDebugMessage.

## Running the Sample

Build the project, then run the Unreal Editor. Click play and watch the debug text at the top left (or the message log).

All supertalk-relevant code is within ASampleSupertalkPlayer. The script that is being executed is /Game/SampleScript, the source to which
can be found in Content/SampleScript.sts.