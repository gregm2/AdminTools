#ifdef SERVER
#ifdef GAMELABS

SparkPlug g_neo_dodgeball_icon;

void NEODodgeBall_update_icon ()
{
    // perform icon set up here as well
    // force initial load of config
    NEODodgeBall_ConfigData neo_dodgeball_cd = NEODodgeBall_Config.GetConfigData();
    if(!neo_dodgeball_cd)
    {
        Print("unable to create dodgeball config data");
    }
    else
    {
        if (neo_dodgeball_cd.show_icon)
        {
            if (!g_neo_dodgeball_icon)
            {
                int flags = ECE_SETUP | ECE_UPDATEPATHGRAPH | ECE_CREATEPHYSICS | ECE_NOLIFETIME | ECE_DYNAMIC_PERSISTENCY;
                g_neo_dodgeball_icon = SparkPlug.Cast(g_Game.CreateObjectEx("SparkPlug", neo_dodgeball_cd.icon_position, flags, RF_IGNORE));
            }
            if (g_neo_dodgeball_icon)
            {
                g_neo_dodgeball_icon.NEODodgeBall_SetIcon();
            }
        }
        else
        {
            if (g_neo_dodgeball_icon)
            {
                g_neo_dodgeball_icon.Delete();
                g_neo_dodgeball_icon = NULL;
            }
        }
    }
};

modded class MissionServer
{
    override void GLActionRegisterHook() 
    {
        super.GLActionRegisterHook();

        NEODodgeBall_ConfigMod().Register();
        NEODodgeBall_update_icon();
    }
};


// create config actions for mod
class NEODodgeBall_ConfigMod : GameLabsContextAction
{
    void NEODodgeBall_ConfigMod()
    {
        this.actionCode = "NEODodgeBall_ConfigMod";
        this.actionName = "Config Deadly Dodgeball Mod";
        this.actionIcon = "volleyball-ball";
        this.actionColour = "default";
        this.actionContext = "world";
        
        // select what to configure
        GameLabsActionParameter configItem = new GameLabsActionParameter("Config setting", "Select which item to set","options");

        // config options
        GameLabsActionParameter setdbtype = new GameLabsActionParameter("Set DodgeBall Type", "What item becomes a dodgeball","string");
        setdbtype.valueString = "setdbtype";
        configItem.options.Insert("setdbtype",setdbtype);
        
        GameLabsActionParameter setcdelay = new GameLabsActionParameter("Set Consequence Delay ms", "milliseconds to delay consequence so dying player hears sounds, 0 is valid", "string");
        setcdelay.valueString = "setcdelay";
        configItem.options.Insert("setcdelay", setcdelay);
        
        GameLabsActionParameter setc = new GameLabsActionParameter("Set Consequence", "must be 'death','legbreak', or 'uncon'", "string");
        setc.valueString = "setc";
        configItem.options.Insert("setc", setc);
        
        GameLabsActionParameter setss = new GameLabsActionParameter("Set SoundSet", "can be 'contamination' a valid soundset name or empty for no sound", "string");
        setss.valueString = "setss";
        configItem.options.Insert("setss", setss);
        
        GameLabsActionParameter setssd = new GameLabsActionParameter("Set SoundSet Duration ms", "millisecond delay before halting soundset to prevent looping", "string");
        setssd.valueString = "setssd";
        configItem.options.Insert("setssd", setssd);
        
        GameLabsActionParameter enableunconspec = new GameLabsActionParameter("Enable spectate on uncon", "(experimental) Players unconned by dodgeball get to spectate", "string");
        enableunconspec.valueString = "enableunconspec";
        configItem.options.Insert("enableunconspec", enableunconspec);
        
        GameLabsActionParameter disableunconspec = new GameLabsActionParameter("Disable spectate on uncon", "Players unconned by dodgeball don't spectate","string");
        disableunconspec.valueString = "disableunconspec";
        configItem.options.Insert("disableunconspec", disableunconspec);
        
        GameLabsActionParameter setIconPosition = new GameLabsActionParameter("Set icon position", "moves mod info icon", "string");
        setIconPosition.valueString = "setIconPosition";
        configItem.options.Insert("setIconPosition", setIconPosition);
        
        GameLabsActionParameter showIcon = new GameLabsActionParameter("Show icon", "displays icon on map","string");
        showIcon.valueString = "showIcon";
        configItem.options.Insert("showIcon", showIcon);
        
        GameLabsActionParameter hideIcon = new GameLabsActionParameter("Hide icon", "hides icon on map", "string");
        hideIcon.valueString = "hideIcon";
        configItem.options.Insert("hideIcon", hideIcon);

        this.parameters.Insert("configItem", configItem);        


        // editable fields
        GameLabsActionParameter dbtype = new GameLabsActionParameter("Dodgeball Type", "what items are dodgeballs", "cf_itemlist");
        this.parameters.Insert("dbtype", dbtype);
        
        GameLabsActionParameter int_ms_val = new GameLabsActionParameter("ConsequenceDelay/SoundSetDuration", "milliseconds value","int");
        int_ms_val.valueInt = 250;
        this.parameters.Insert("int_ms_val", int_ms_val);
        
        GameLabsActionParameter c_str_val = new GameLabsActionParameter("Consequence", "must be 'death, 'uncon', or 'legbreak'","string");
        c_str_val.valueString = "death";
        this.parameters.Insert("c_str_val", c_str_val);

        GameLabsActionParameter ss_name = new GameLabsActionParameter("SoundSet name", "empty = no sound, 'contamination' = gas sound, or set to any valid soundset","string");
        ss_name.valueString = "contamination";
        this.parameters.Insert("ss_name", ss_name);
        
        GameLabsActionParameter icon_vector = new GameLabsActionParameter("Icon Position", "modinfo icon position on map","vector");
        this.parameters.Insert("icon_vector",icon_vector);
    }

    override bool Execute(GameLabsActionContext context)
    {
        string configitem = context.parameters.Get("configItem").GetString();
        
        if (configitem == "")
        {
            return true;
        }
        else if (configitem == "setdbtype")
        {
            NEODodgeBall_Config.SetDodgeBallType(context.parameters.Get("dbtype").GetString());
        }
        else if (configitem == "setcdelay")
        {
            NEODodgeBall_Config.SetConsequenceDelayMs(context.parameters.Get("int_ms_val").GetInt());
        }
        else if (configitem == "setc")
        {
            NEODodgeBall_Config.SetConsequence(context.parameters.Get("c_str_val").GetString());
        }
        else if (configitem == "setss")
        {
            NEODodgeBall_Config.SetSoundSet(context.parameters.Get("ss_name").GetString());
        }
        else if (configitem == "setssd")
        {
            NEODodgeBall_Config.SetSoundSetDurationMs(context.parameters.Get("int_ms_val").GetInt());
        }
        else if (configitem == "enableunconspec")
        {
            NEODodgeBall_Config.SetSpectateUncon(true);
        }
        else if (configitem == "disableunconspec")
        {
            NEODodgeBall_Config.SetSpectateUncon(false);
        }
        else if (configitem == "setIconPosition")
        {
            NEODodgeBall_Config.SetIconPosition(context.parameters.Get("icon_vector").GetVector());
        }
        else if (configitem == "showIcon")
        {
            NEODodgeBall_Config.SetShowIcon(true);
        }
        else if (configitem == "hideIcon")
        {
            NEODodgeBall_Config.SetShowIcon(false);
        }
        NEODodgeBall_update_icon();
        return true;
    }
};

#endif // GAMELABS
#endif // SERVER