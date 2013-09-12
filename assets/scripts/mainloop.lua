-- Main game loop
function Main()

	-- This is technically not needed at the start of main, but it's included
	-- here to remind Playground users to call it before they call Load() again,
	-- so that their progress bar will work correctly.
	LoaderReset();

	-- Push the main game screen
	while true do
		DoMainWindow("scripts/game.lua");
		-- DoMainWindow will exit only if there are NO windows pushed on the stack, so
		-- a PopModal()/PushModal() combination will not cause this to loop.
	end
end

-- Return a function to be executed in a thread
return Main