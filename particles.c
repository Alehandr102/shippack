object Particles;
// LEO: тот же код, что PS_CREATE_NO_HEAT в native shared/messages.h.
#define FIRE_HEAT_CREATE_NO_HEAT 10015

void InitParticles()
{
    CannonHeatHaze_ApplyConfig();
	Particles.Server = false;
	Particles.windpower = 1.0;
	Particles.winddirection.x = frnd();
	Particles.winddirection.z = frnd();
	if (!CreateParticleEntity())
	{
		Trace("Can't create 'particles' class");
	}
}

void PauseParticles(bool bPause)
{
	if (!CreateParticleEntity()) return;
	SendMessage(&Particles, "ll", PS_PAUSEALL, bPause);
}

void DeleteParticles()
{
	SendMessage(&Particles,"l", PS_CLEARALL);
	Particles.winddirection.x = frnd();
	Particles.winddirection.z = frnd();
}

void DeleteParticleSystem(int id)
{
	SendMessage(&Particles,"ll",PS_DELETE,id);
}


int CreateParticleSystem(string name,float x,float y,float z,
		float ax,float ay,float az,int lifetime)
{
	int pid;
	if (!CreateParticleEntity()) return 0;
	Particles.max = GetParticlesCount();
	pid = SendMessage(&Particles,"lsffffffl",PS_CREATE,name,x,y,z,ax,ay,az,lifetime);
	return pid;
}

bool CreateParticleEntity()
{
	if(IsEntity(Particles) == false)
	{
		if (CreateEntity(&Particles,"particles") == false) return false;
		LayerAddObject(REALIZE,Particles,65536);
		LayerAddObject(EXECUTE,Particles,0);
		LayerAddObject(SEA_REALIZE,Particles,65536);
		LayerAddObject(SEA_EXECUTE,Particles,0);
	}
	return true;
}

// LEO: только настоящая пушка: RealFire уже создал жару у дульного среза.
int CreateParticleSystemNoHeat(string name,float x,float y,float z,
        float ax,float ay,float az,int lifetime)
{
    if (!CreateParticleEntity()) return 0;
    Particles.max = GetParticlesCount();
    return SendMessage(&Particles,"lsffffffl",FIRE_HEAT_CREATE_NO_HEAT,name,x,y,z,ax,ay,az,lifetime);
}

void MoveParticlesToLayers(int iExecuteLayer, int iRealizeLayer)
{
	LayerDelObject(EXECUTE, &Particles);
	LayerDelObject(REALIZE, &Particles);
	LayerDelObject(SEA_EXECUTE, &Particles);
	LayerDelObject(SEA_REALIZE, &Particles);

	LayerAddObject(iExecuteLayer, &Particles, 0);
	LayerAddObject(iRealizeLayer, &Particles, 65536);
}

int CreateParticleSystemX(string name,float x,float y,float z,
		float ax,float ay,float az,int lifetime)
{
	int pid;
	if (!CreateParticleEntity()) return false;
	Particles.max = GetParticlesCount();
	pid = SendMessage(&Particles,"lsffffffl",PS_CREATEX,name,x,y,z,ax,ay,az,lifetime);
	return pid;
}

// fAngDevCorCoef +- 0.1 ... 1./
// fSpeedCorCoef +- 0.001 ... 0.0/
// section название секции в ini\\particles\\particles.ini
int CreateBlast(float x, float y, float z, float fAngDevCorCoef, float fSpeedCorCoef, string section)
{
	int iRes;
	object blast;
	iRes = CreateEntity(&blast,"blast");
	SendMessage(&blast,"lfffffs",LM_SETPOINT,x,y,z,fAngDevCorCoef,fSpeedCorCoef,section);
	LayerAddObject(REALIZE,blast,3);
	LayerAddObject(SEA_REALIZE,blast,3);
	return iRes;
}

int CreateBlastX(float x,float y,float z,float ax,float ay,float az, float fAngDevCorCoef, float fSpeedCorCoef, string section)
{
	int iRes;
	object blast;
	iRes = CreateEntity(&blast,"blast");
	SendMessage(&blast,"lffffffffs",LM_SETPOINTANDANGLES,x,y,z,ax,ay,az,fAngDevCorCoef,fSpeedCorCoef,section);
	LayerAddObject(REALIZE,blast,3);
	LayerAddObject(SEA_REALIZE,blast,3);
	return iRes;
}