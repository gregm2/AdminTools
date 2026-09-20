#ifdef SERVER
#ifdef GAMELABS

// using spark plug objects for cftools to track and display icon
// chosen since it doesn't freeze/rot/or other processing
modded class SparkPlug
{
    ref _Event NeoDodgeBall_cftools_icon;

    void ~SparkPlug()
    {
        if (!NeoDodgeBall_cftools_icon)
        {
            return;
        }
        GameLabsCore glc = GetGameLabs();
        if (glc)
        {
            glc.RemoveEvent(NeoDodgeBall_cftools_icon);
        }
    }

    // call this after spawning the sparkplug to display as mod icon
    void NEODodgeBall_SetIcon()
    {
        GameLabsCore glc = GetGameLabs();
        if (!glc)
        {
            Print("[neo_dodgeball_icon.c] Unable to get GameLabsCore reference");
            return;
        }
        
        if (NeoDodgeBall_cftools_icon)
        {
            glc.RemoveEvent(NeoDodgeBall_cftools_icon);
            NeoDodgeBall_cftools_icon = NULL;
        }
        
        NEODodgeBall_ConfigData configdata = NEODodgeBall_Config.GetConfigData();
        if (!configdata)
        {
            Print("[neo_dodgeball_icon.c] Unable to get config");
            return;
        }

        string modinfo = "<ul>";

        string temp = "<li>DodgeBallType: None</li>"
        if (configdata.dodgeball_type != "")
        {
            temp = string.Format("<li>DodgeBallType: %1</li>",configdata.dodgeball_type);
        }
        modinfo += temp;

        modinfo += string.Format("<li>ConsequenceDelay_ms: %1</li>", configdata.consequence_delay_ms);

        modinfo += string.Format("<li>Consequence: %1</li>", configdata.consequence);

        temp = "<li>SoundSet: None</li>";
        if (configdata.soundset != "")
        {
            temp = string.Format("<li>SoundSet: %1</li>", configdata.soundset);
        }
        modinfo += temp;

        modinfo += string.Format("<li>SoundSetDuration_ms: %1</li>", configdata.soundset_duration_ms);

        temp = "<li>SpectateUncon: false</li>";
        if (configdata.spectate_uncon)
        {
            temp = string.Format("<li>SpectateUncon: true</li>");
        }
        modinfo += temp;
        modinfo += "</ul>"

        if (glc)
        {
            NeoDodgeBall_cftools_icon = new _Event("DodgeBallModInfo", "volleyball", this, modinfo);
            if (NeoDodgeBall_cftools_icon)
            {
                glc.RegisterEvent(this.NeoDodgeBall_cftools_icon);
            }
            else
            {
                Print("[neo_dodgeball_icon.c] Unable to create gamelabs _Event object");
            }
        }
    }
};

#endif // GAMELABS
#endif // SERVER