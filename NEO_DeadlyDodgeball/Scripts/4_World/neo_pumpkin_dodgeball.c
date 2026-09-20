#ifdef SERVER

// mostly using the old easter egg code to determine when thrown and then we kill on contact or touch of living player
// https://github.com/BohemiaInteractive/DayZ-Script-Diff/blob/a22a0553779b157f172283233e91933cc8ee2102/scripts/4_world/entities/itembase/gear/consumables/easteregg.c#L117C1-L135C3

bool NEO_DodgeBallDebug = false;
float NEO_DodgeBall_min_velocity = 0.3; //dodge balls aren't deadly below this velocity

void NEODodgeBall_consequence(DayZPlayerImplement dzpi, string consequence_type)
{
    PlayerBase pb;
    if (dzpi)
    {
        if (consequence_type == "death")
        {
            dzpi.SetHealth("","",0.0);
        }
        else if (consequence_type == "uncon")
        {
            dzpi.neo_dodgeball_unconned = true;
            pb  = PlayerBase.Cast(dzpi);
            if (pb)
            {
                pb.GiveShock(-100);
            }
        }
        else if (consequence_type == "legbreak")
        {
            pb = PlayerBase.Cast(dzpi);
            if(pb)
            {
                pb.SetLegHealth();
                if (pb.GetModifiersManager().IsModifierActive(eModifiers.MDF_BROKEN_LEGS))//effectively resets the modifier
                {
                    pb.GetModifiersManager().DeactivateModifier(eModifiers.MDF_BROKEN_LEGS);
                }
                pb.GetModifiersManager().ActivateModifier(eModifiers.MDF_BROKEN_LEGS);
            }
        }
        else
        {
            Print(string.Format("Neododgeball invalid consequence %1",consequence_type));
        }
    }
};

void NEODodgeBall_playsound(ItemBase dodgeball, string soundset, int duration)
{
    if (!dodgeball || (soundset == ""))
    {
        return;
    }
    
    if (soundset == "contamination")
    {
        // drop a contamination RPC on dodgeball position
        Param1<vector> pos = new Param1<vector>(vector.Zero);
        array<ref Param> params = new array<ref Param>();
        pos.param1 = dodgeball.GetPosition();
        params.Insert(pos);
        g_Game.RPC(null, ERPCs.RPC_SOUND_CONTAMINATION, params, true);
    }
    else if (duration > 0)
    {
        Param2<bool, string> play = new Param2<bool, string>(true, soundset);
        g_Game.RPCSingleParam(dodgeball, ERPCs.RPC_SOUND_LOCK_ATTACH, play, true );
        Param2<bool, string> stopplay = new Param2<bool, string>(false, soundset);
        g_Game.GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater(g_Game.RPCSingleParam, duration, false, dodgeball,ERPCs.RPC_SOUND_LOCK_ATTACH, stopplay, true  );
    }
};



void NEODodgeBall_handle_kill(DayZPlayerImplement dzpi, ItemBase ib)
{
    if (dzpi)
    {
        PlayerIdentity pi = dzpi.GetIdentity();
        vector dzpi_pos = dzpi.GetPosition();

        PlayerBase victim = PlayerBase.Cast(dzpi);
        PlayerBase killer = ib.NEO_dodgeball_thrower;
        // verify real players for logging
        string vic_pi;
        string kil_pi;
        if (victim && killer)
        {
            PlayerIdentity temp_pi = victim.GetIdentity();
            if (temp_pi)
            {
                vic_pi = temp_pi.GetPlainId();
            }
            temp_pi = killer.GetIdentity();
            if (temp_pi)
            {
                kil_pi = temp_pi.GetPlainId();
            }
        }

        // log to gamelabs
        #ifdef GAMELABS
        if (victim && killer && vic_pi && kil_pi)
        {
            _Payload_PlayerDeath payload;
            _LogPlayerEx logplayervictim = new _LogPlayerEx(victim);
            _LogPlayerEx logplayerkiller = new _LogPlayerEx(killer);
            payload = new _Payload_PlayerDeath(logplayervictim, logplayerkiller, "DodgeBall", "DodgeBall");
            GetGameLabs().GetApi().PlayerDeath(new _Callback(), payload);
        }
        #endif

        // log to admin log
        PluginAdminLog adm = PluginAdminLog.Cast(GetPlugin(PluginAdminLog));
        if (adm)
        {
            if (victim && killer && vic_pi && kil_pi)
            {
                string vic_prefix = adm.GetPlayerPrefix(victim, victim.GetIdentity());
                string kil_prefix = adm.GetPlayerPrefix(killer, killer.GetIdentity());
                float dist = vector.Distance(victim.GetPosition(), killer.GetPosition());
                adm.LogPrint( vic_prefix + " killed by " + kil_prefix + " with DodgeBall" + " from " + dist + " meters " );
            }
        }
        
        NEODodgeBall_ConfigData ncd = NEODodgeBall_Config.GetConfigData();
        if (ncd)
        {
            if (ncd.consequence_delay_ms > 0)
            {
                // dying player dies 500 ms later so they can see/hear FX
                g_Game.GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( NEODodgeBall_consequence, ncd.consequence_delay_ms, false, dzpi, ncd.consequence);
            }
            else
            {
                NEODodgeBall_consequence(dzpi, ncd.consequence);
            }
            NEODodgeBall_playsound(ib, ncd.soundset, ncd.soundset_duration_ms);
        }
    }
};


modded class DayZPlayerImplement extends DayZPlayer
{
    bool neo_dodgeball_unconned = false;

    void NEO_player_contact_dodgeball(IEntity other)
    {
        if (other)
        {
            ItemBase ib = ItemBase.Cast(other);
            if (ib)
            {
                float velocity = GetVelocity(ib).Length();
                if (velocity < NEO_DodgeBall_min_velocity)
                {
                    ib.NEO_i_am_a_dodgeball_now = false;
                    return;
                }
                if (ib.NEO_i_am_a_dodgeball_now)
                {
                    NEODodgeBall_handle_kill(this, ib);
                    ib.NEO_i_am_a_dodgeball_now = false;
                }
            }
        }
    }
    
    // hit other entities
    override void EOnTouch( IEntity other, int extra )
    {
        if (NEO_DodgeBallDebug)
        {
            Object ob = Object.Cast(other);
            GetGame().AdminLog(string.Format("In DayZPlayerImplement::EOnTouch in dodgeball mod, other type: %1", ob.GetType()));
        }
        NEO_player_contact_dodgeball(other);
    }
    
    // hit things like the ground (gonna implement just in case)
    override void EOnContact( IEntity other, Contact extra )
    {
        if (NEO_DodgeBallDebug)
        {
            Object ob = Object.Cast(other);
            GetGame().AdminLog(string.Format("In DayZPlayerImplement::EOnContact in dodgeball mod, other type: %1", ob.GetType()));
        }
        NEO_player_contact_dodgeball(other);
    }
};

modded class ItemBase
{
    bool NEO_i_am_a_dodgeball_now = false;
    PlayerBase NEO_dodgeball_thrower;
    
    void NEO_DodgeBallContact(IEntity other)
    {
        if (NEO_DodgeBallDebug)
        {
            GetGame().AdminLog("In NEO_DodgeBallContact");
        }
        if (NEO_i_am_a_dodgeball_now)
        {
            if (NEO_DodgeBallDebug)
            {
                GetGame().AdminLog("ItemBase is dodgeball");
            }
            // if not moving (velocity less than .2 for now, then no longer deadly), 
            // otherwise I think ppl will die picking up a stopped 'ball'
            float velocity = GetVelocity(this).Length();
            if (velocity < NEO_DodgeBall_min_velocity)
            {
                if (NEO_DodgeBallDebug)
                {
                    GetGame().AdminLog(string.Format("NEO_Dodgeball Velocity too low = %1", velocity));
                }
                NEO_i_am_a_dodgeball_now = false;
                return;
            }
        }
    }
    
    // hit other entities
    override void EOnTouch( IEntity other, int extra )
    {
        if (NEO_DodgeBallDebug)
        {
            Object ob = Object.Cast(other);
            GetGame().AdminLog(string.Format("In EOnTouch in dodgeball mod, other type: %1", ob.GetType()));
        }
        NEO_DodgeBallContact(other);
    }
    
    // hit things like the ground (gonna implement just in case)
    override void EOnContact( IEntity other, Contact extra )
    {
        if (NEO_DodgeBallDebug)
        {
            Object ob = Object.Cast(other);
            GetGame().AdminLog(string.Format("In EOnContact in dodgeball mod, other type: %1", ob.GetType()));
        }
        NEO_DodgeBallContact(other);
    }
    
    override void OnInventoryExit( Man player )
    {
        if (NEO_DodgeBallDebug)
        {
            GetGame().AdminLog("In OnInventoryExit in dodgeball mod");
        }
        super.OnInventoryExit(player);
        NEO_i_am_a_dodgeball_now = false;
        
        NEODodgeBall_ConfigData ncd = NEODodgeBall_Config.GetConfigData();
        
        PlayerBase p = PlayerBase.Cast( player );
        if (p)
        {
            DayZPlayerImplementThrowing player_throwing = p.GetThrowing();
            if (player_throwing)
            {
                if (player_throwing.IsThrowingAnimationPlaying())
                {
                    if (ncd && this.IsKindOf(ncd.dodgeball_type))
                    {
                        if (NEO_DodgeBallDebug)
                        {
                            GetGame().AdminLog("Is deadly dodge ball now");
                        }
                        NEO_i_am_a_dodgeball_now = true;
                        NEO_dodgeball_thrower = p;
                    }
                }
                else
                {
                    if (NEO_DodgeBallDebug)
                    {
                        GetGame().AdminLog("was not in throwing animation");
                    }
                }
            }
            else
            {
                if (NEO_DodgeBallDebug)
                {
                    GetGame().AdminLog("wasn't being thrown");
                }
            }
        }
        else
        {
            if (NEO_DodgeBallDebug)
            {
                GetGame().AdminLog("didn't exit player inventory");
            }
        }
    }
};


#endif // SERVER