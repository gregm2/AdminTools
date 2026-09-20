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

        string modinfo = "";

        string temp = "DodgeBallType: None";
        if (configdata.dodgeball_type != "")
        {
            temp = string.Format("DodgeBallType: %1",configdata.dodgeball_type);
        }
        modinfo += temp;

        modinfo += string.Format("<br/>ConsequenceDelay_ms: %1", configdata.consequence_delay_ms);

        modinfo += string.Format("<br/>Consequence: %1", configdata.consequence);

        temp = "<br/>SoundSet: None";
        if (configdata.soundset != "")
        {
            temp = string.Format("<br/>SoundSet: %1", configdata.soundset);
        }
        modinfo += temp;

        modinfo += string.Format("<br/>SoundSetDuration_ms: %1", configdata.soundset_duration_ms);

        temp = "<br/>SpectateUncon: false";
        if (configdata.spectate_uncon)
        {
            temp = string.Format("<br/>SpectateUncon: true");
        }
        modinfo += temp;
        

        if (glc)
        {
            NeoDodgeBall_cftools_icon = new _Event("DodgeBallModInfo", "volleyball-ball", this, modinfo);
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