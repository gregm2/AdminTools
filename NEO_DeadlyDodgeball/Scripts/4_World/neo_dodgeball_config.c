#ifdef SERVER


class NEODodgeBall_ConfigData
{
    // dodgeball_type:       can be an object type or even parent class (i.e. DeadChicken_ColorBase)
    // consequence_delay_ms: delay death etc. so dying player hears sound (0 is valid for no delay)
    // consequence:          must be 'death', 'legbreak', or 'uncon'
    // soudset:              'contamination' for gas sound, soundset name, or empty string for no sound
    // soundset_duration_ms: soundsets will loop if not turned off, this is delay for turn off RPC
    // spectate_uncon:       (experimental) if uncon, put player in spectate mode to allow them to keep watching
    // icon_position:        position of icon in CFtools diplaying mod config info
    // show_icon             boolean to display icon or not in cftools

    string  dodgeball_type          = "pumpkin";
    int     consequence_delay_ms    = 500;
    string  consequence             = "death";
    string  soundset                = "contamination";
    int     soundset_duration_ms    = 500;
    bool    spectate_uncon          = false;    
    vector  icon_position           = "7700 0 1350";
    bool    show_icon               = true;
}

class NEODodgeBall_Config
{
    static ref NEODodgeBall_ConfigData g_NeoDodgeBall_configdata;
    static bool config_file_loaded = false;
    static string config_filename = "$profile:NEO_DeadlyDodgeball_config.json";

    static void SetDefault(NEODodgeBall_ConfigData configdata)
    {
        if (!configdata)
        {
            Print("[neo_dodgeball_config.c] can't set default values on NULL config object");
            return;
        }
        configdata.dodgeball_type = "pumpkin";
        configdata.consequence_delay_ms = 500;
        configdata.consequence = "death";
        configdata.soundset = "contamination";
        configdata.soundset_duration_ms = 500;
        configdata.spectate_uncon = false;
        configdata.icon_position = "7700 0 1350";
        configdata.show_icon = true;

        NEODodgeBall_Config.StoreConfig(configdata);
    }

    static NEODodgeBall_ConfigData GetConfigData()
    {
        if (!NEODodgeBall_Config.g_NeoDodgeBall_configdata)
        {
            NEODodgeBall_Config.g_NeoDodgeBall_configdata = new NEODodgeBall_ConfigData;
            if (NEODodgeBall_Config.g_NeoDodgeBall_configdata)
            {
                NEODodgeBall_Config.LoadConfig(NEODodgeBall_Config.g_NeoDodgeBall_configdata);
            }
            else
            {
                Print("]neo_dodgeball_config.c] Failed to allocation config data object");
                // null result returned to caller
            }
        }
        return NEODodgeBall_Config.g_NeoDodgeBall_configdata;
    }

    static void StoreConfig(NEODodgeBall_ConfigData configdata)
    {
        if (!configdata)
        {
            Print("[neo_dodgeball_config.c] Can't store config data NULL pointer");
            return;
        }
        string filename = NEODodgeBall_Config.config_filename;
        string errormsg;
        bool result;
        result = JsonFileLoader<NEODodgeBall_ConfigData>.SaveFile(filename, configdata, errormsg);
        if (!result)
        {
            Print("[neo_dodgeball_config.c] Unable to save config: "+errormsg);
        }
    }

    static void LoadConfig(out NEODodgeBall_ConfigData configdata)
    {
        string errormsg;
        bool result;
        string filename = NEODodgeBall_Config.config_filename;
        if (!FileExist(filename))
        {
            // no config has been saved, normal on first run if admin made no config file
            return;
        }
        if (!configdata)
        {
            Print("[neo_dodgeball_config.c] Unable to load config data into NULL pointer");
            return;
        }
        result = JsonFileLoader<NEODodgeBall_ConfigData>.LoadFile(filename, configdata, errormsg);
        if (!result)
        {
            Print("[neo_dodgeball_config.c] Load failed, setting defaults, error: " + errormsg);
            NEODodgeBall_Config.SetDefault(configdata);
        }
    }

    static void SetDodgeBallType (string dbtype)
    {
        NEODodgeBall_ConfigData configdata = NEODodgeBall_Config.GetConfigData();
        if (!configdata)
        {
            Print("[neo_dodgeball_config.c] Unable to set data on NULL config object");
            return;
        }
        dbtype.ToLower(); // standardize on lower case strings for checks, iskindof() does this anyway
        configdata.dodgeball_type = dbtype; // too many possibilities to check validity here
        NEODodgeBall_Config.StoreConfig(configdata);
    }

    static void SetConsequenceDelayMs (int delay)
    {
        NEODodgeBall_ConfigData configdata = NEODodgeBall_Config.GetConfigData();
        if (!configdata)
        {
            Print("[neo_dodgeball_config.c] Unable to set data on NULL config object");
            return;
        }
        if (delay < 0)
        {
            Print("[neo_dodgeball_config.c] Consequence delay can't be negative");
            return;
        }
        configdata.consequence_delay_ms = delay;
        NEODodgeBall_Config.StoreConfig(configdata);
    }

    static void SetConsequence (string consequence)
    {
        NEODodgeBall_ConfigData configdata = NEODodgeBall_Config.GetConfigData();
        if (!configdata)
        {
            Print("[neo_dodgeball_config.c] Unable to set data on NULL config object");
            return;
        }
        consequence.ToLower();
        if ((consequence != "death") && (consequence != "legbreak") && (consequence != "uncon"))
        {
            Print("[neo_dodgeball_config.c] Consequence must be one of 'death', 'legbreak', or 'uncon'");
            return;
        }
        configdata.consequence = consequence;
        NEODodgeBall_Config.StoreConfig(configdata);
    }

    static void SetSoundSet (string soundset)
    {
        NEODodgeBall_ConfigData configdata = NEODodgeBall_Config.GetConfigData();
        if (!configdata)
        {
            Print("[neo_dodgeball_config.c] Unable to set data on NULL config object");
            return;
        }
        // too many possibilities to validate, set to lower for faster checking for contamination
        // empty string = no sound and is valid too
        soundset.ToLower();
        configdata.soundset = soundset;
        NEODodgeBall_Config.StoreConfig(configdata);
    }

    static void SetSoundSetDurationMs (int duration)
    {
        NEODodgeBall_ConfigData configdata = NEODodgeBall_Config.GetConfigData();
        if (!configdata)
        {
            Print("[neo_dodgeball_config.c] Unable to set data on NULL config object");
            return;
        }
        if (duration < 0)
        {
            Print("[neo_dodgeball_config.c] Soundset duration can't be negative");
            return;
        }
        configdata.soundset_duration_ms = duration;
        NEODodgeBall_Config.StoreConfig(configdata);
    }

    static void SetSpectateUncon (bool enable)
    {
        NEODodgeBall_ConfigData configdata = NEODodgeBall_Config.GetConfigData();
        if (!configdata)
        {
            Print("[neo_dodgeball_config.c] Unable to set data on NULL config object");
            return;
        }
        configdata.spectate_uncon = enable;
        NEODodgeBall_Config.StoreConfig(configdata);
    }

    static void SetIconPosition (vector position)
    {
        NEODodgeBall_ConfigData configdata = NEODodgeBall_Config.GetConfigData();
        if (!configdata)
        {
            Print("[neo_dodgeball_config.c] Unable to set data on NULL config object");
            return;
        }
        position[1] = 0;
        if ((position[0] < 0) || (position[2] < 0))
        {
            Print("[neo_dodgeball_config.c] X and Y coordinates for icon position must be positive");
            return;
        }
        configdata.icon_position = position;
        NEODodgeBall_Config.StoreConfig(configdata);
    }

    static void SetShowIcon(bool enable)
    {
        NEODodgeBall_ConfigData configdata = NEODodgeBall_Config.GetConfigData();
        if (!configdata)
        {
            Print("[neo_dodgeball_config.c] Unable to set data on NULL config object");
            return;
        }
        configdata.show_icon = enable;
        NEODodgeBall_Config.StoreConfig(configdata);
    }
};

#endif // SERVER