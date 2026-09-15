#ifdef SERVER

// mostly using the old easter egg code to determine when thrown and then we kill on contact or touch of living player
// https://github.com/BohemiaInteractive/DayZ-Script-Diff/blob/a22a0553779b157f172283233e91933cc8ee2102/scripts/4_world/entities/itembase/gear/consumables/easteregg.c#L117C1-L135C3

bool NEO_DodgeBallDebug = false;
float NEO_DodgeBall_min_velocity = 0.3; //dodge balls aren't deadly below this velocity
float NEO_DodgeBall_fake_sound_delay = 10; // see RPC handler for RPC_SOUND_ARTILLERY_SINGLE delay before calllater


void NEODodgeBall_delay_kill_player(DayZPlayerImplement dzpi)
{
    if (dzpi)
    {
        dzpi.SetHealth("","",0.0);
    }
}


void NEODodgeBall_handle_kill(DayZPlayerImplement dzpi, Pumpkin pumpkin)
{
    if (dzpi)
    {
        PlayerIdentity pi = dzpi.GetIdentity();
        vector dzpi_pos = dzpi.GetPosition();

        PlayerBase victim = PlayerBase.Cast(dzpi);
        PlayerBase killer = pumpkin.NEO_dodgeball_thrower;
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


        // drop a contamination RPC on the dying player
        Param1<vector> pos = new Param1<vector>(vector.Zero);
        array<ref Param> params = new array<ref Param>();
        pos.param1 = dzpi_pos;
        params.Insert(pos);
        g_Game.RPC(null, ERPCs.RPC_SOUND_CONTAMINATION, params, true);

        // dying player dies 500 ms later so they can see/hear FX
        g_Game.GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( NEODodgeBall_delay_kill_player, 500, false, dzpi);
    }
}


modded class DayZPlayerImplement extends DayZPlayer
{
    void NEO_player_contact_dodgeball(IEntity other)
    {
        if (other)
        {
            Pumpkin p = Pumpkin.Cast(other);
            if (p)
            {
                float velocity = GetVelocity(p).Length();
                if (velocity < NEO_DodgeBall_min_velocity)
                {
                    p.NEO_i_am_a_dodgeball_now = false;
                    return;
                }
                if (p.NEO_i_am_a_dodgeball_now)
                {
                    NEODodgeBall_handle_kill(this, p);
                    p.NEO_i_am_a_dodgeball_now = false;
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
}

modded class Pumpkin : Edible_Base
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
                GetGame().AdminLog("Pumpkin is dodgeball");
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
        
        PlayerBase p = PlayerBase.Cast( player );
        if (p)
        {
            DayZPlayerImplementThrowing player_throwing = p.GetThrowing();
            if (player_throwing)
            {
                if (player_throwing.IsThrowingAnimationPlaying())
                {
                    if (NEO_DodgeBallDebug)
                    {
                        GetGame().AdminLog("Is deadly dodge ball now");
                    }
                    NEO_i_am_a_dodgeball_now = true;
                    NEO_dodgeball_thrower = p;
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
}


#endif // SERVER