/****************************** Header ******************************\
Class Name: Robot inherits SampleRobot
File Name: Robot.cpp
Summary: Entry point from WPIlib, main class where robot routines are
started.
Project: BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Ryan Cooper, Dylan Watson, Chris Weeks
Email: cooper.ryan@centaurisoftware.co, dylantrwatson@gmail.com, 
chrisrweeks@aol.com
\********************************************************************/

#include <iostream>
#include "Robot.h"
//TODO: Potentiometer include, VictorSPX include
//TODO: Logging class (will assign to Ian in TFS at some point soon. Will be done in dev, does not require new branch)
using namespace std;

/**
 * Constructor
 */
Robot::Robot() 
{
	m_drive = new Drive();
	m_activeCollection = new ActiveCollection();
	//extend the life of the active region's copy by adding a reference to this variable
	m_EventMap = m_activeCollection->GetEventMap_Shared();
}

Robot::~Robot()
{
	delete m_drive;
	m_drive = nullptr;
	delete m_activeCollection;
	m_activeCollection = nullptr;
}

/*
 * Initialization method of the robot
 * This runs right after Robot() when the code is started
 * Creates Config
 */
void Robot::RobotInit()
{
	Log::restartfile();
	cout << "Program Version: " << VERSION << " Revision: " << REVISION << endl;
	//CameraServer::GetInstance()->StartAutomaticCapture(0);
	#ifdef __UseXMLConfig__
	Config *config = new Config(m_activeCollection, m_drive); //!< Pointer to the configuration file of the robot
	#endif
	//Must have this for smartdashboard to work properly
	SmartDashboard::init();
	//Select which lua to load here
	#ifdef _Win32
	const char *RobotScript = "FRC2019_Simulation.lua";
	//const char *RobotScript = "FRC2019Robot.lua";
	#else
	const char *RobotScript = "FRC2019Robot.lua";
	#endif	
	m_Robot.RobotAssem_init(RobotScript,&m_activeCollection->GetEventMap(), m_activeCollection);
	m_drive->SetUseDrive(m_Robot.get_using_ac_drive());
	m_drive->SetUseOperator(m_Robot.get_using_ac_elevator());
	m_inst = nt::NetworkTableInstance::GetDefault();		  //!Network tables
	m_visionTable = m_inst.GetTable("VISION_2019");			  //!Vision network table
	m_dashboardTable = m_inst.GetTable("DASHBOARD_TABLE");
	m_dashboardTable->PutStringArray("AUTON_OPTIONS", m_autonOptions);
	m_dashboardTable->PutStringArray("POSITION_OPTIONS", m_positionOptions);


	//TODO: put this in some sort of config
	m_visionTable->PutNumber("LS",0);
	m_visionTable->PutNumber("US",3);
	m_visionTable->PutNumber("LH",87);
	m_visionTable->PutNumber("UH",126);
	m_visionTable->PutNumber("LV",255);
	m_visionTable->PutNumber("UV",255);
	m_visionTable->PutNumber("MinA",1112);
	m_visionTable->PutNumber("MaxA",82763);
	m_visionTable->PutNumber("MaxO",62);
}

/*
 * Method that runs at the beginning of the autonomous mode
 * Uses the SmartDashboard to select the proper Autonomous mode to run
  TODO: Integrate with our dashboad; Integrate with TeleOp for sandstorm; make everything that should be abstract is in fact abstract; Integrate vision overlay and drive cam to dashboard; Write teleop goals;
 */

void Robot::Autonomous()
{


	m_masterGoal = new MultitaskGoal_ac(m_activeCollection, false);

	cout << "Autonomous Started." << endl;
	#if 1
	string autoSelected = SmartDashboard::GetString("AutonTest", "NONE");
	//string autoSelected = SmartDashboard::GetString("Auto Selector", m_driveStraight);
	#else
	string autoSelected = m_dashboardTable->GetString("AUTON_SELECTION", m_driveStraight);
	#endif
	string positionSelected = m_dashboardTable->GetString("POSITION_SELECTION", "NONE"); //if it is none, then just drive straight
	cout << autoSelected << endl;
	if (!SelectAuton(m_activeCollection, m_masterGoal, autoSelected, positionSelected))
	{
		m_dashboardTable->PutString("AUTON_FOUND", "UNDEFINED AUTON OR POSITION SELECTED");
	}
	m_masterGoal->AddGoal(new Goal_TimeOut(m_activeCollection, 15.0));
	m_masterGoal->AddGoal(new Goal_ControllerOverride(*m_EventMap));
	m_masterGoal->Activate();
	double dTime = 0.010;
	double current_time = 0.0;
	SmartDashboard::PutBoolean("AutoPilot", true);
	while (m_masterGoal->GetStatus() == Goal::eActive && _IsAutononomous() && !IsDisabled())
	{
		m_masterGoal->Process(dTime);
		SmartDashboard::PutNumber("Timer", 15.0 - current_time);
		current_time += dTime;
		m_Robot.Update(dTime);
		Wait(dTime);
	}
	SmartDashboard::PutBoolean("AutoPilot", false);
	m_masterGoal->~MultitaskGoal_ac();
	cout << "goal loop complete" << endl;
	cout << m_activeCollection->GetNavX()->GetAngle() << endl;
#if 0 //old code
		if (autoSelected == m_driveStraight) //!< Drive Straight Autonomous
		{
			DriveStraight *driveStraight = new DriveStraight(m_activeCollection);
			driveStraight->Start();
		}

		else //!< Default Autonomous
		{
			DriveStraight *driveStraight = new DriveStraight(m_activeCollection);
			driveStraight->Start();
		}
#endif
}

/*
 * Called when teleop starts
 */
void Robot::OperatorControl()
{
	#if 0
	cout << "Teleoperation Started." << endl;
	while (IsOperatorControl() && !IsDisabled())
	{
		m_drive->Update();
		Wait(0.010);
	}
	#else
	Log::restartfile();
	cout << "Teleoperation Started." << endl;
	double LastTime = GetTime();
	//We can test teleop auton goals here a bit later
	while (IsOperatorControl() && !IsDisabled())
	{
		const double CurrentTime = GetTime();
		#ifndef _Win32
		const double DeltaTime = CurrentTime - LastTime;
		#else
		const double DeltaTime=0.01;  //It's best to use sythetic time for simulation to step through code
		#endif
		LastTime = CurrentTime;
		if (DeltaTime == 0.0) continue;  //never send 0 time
		//printf("DeltaTime=%.2f\n",DeltaTime);
		m_Robot.Update(DeltaTime);
		//Depreciated
		#ifdef __UseXMLConfig__
		m_drive->Update();
		#endif
		//using this from test runs from robo wranglers code
		Wait(0.010);
	}
	#endif
}

/*
 * Called when the Test period starts
 */
void Robot::Test()
{
	const double TimeOut=10.0;
	Goal_DriveWithTimer m_masterGoal(m_activeCollection, 0.5, 0.25, TimeOut);
	double dTime = 0.010;
	m_masterGoal.Activate();
	double currentTime=0.0;
	SmartDashboard::PutBoolean("AutoPilot", true);
	while (IsTest() && !IsDisabled() && m_masterGoal.GetStatus()==Goal::eActive)
	{
		SmartDashboard::PutNumber("Timer", TimeOut - currentTime);
		currentTime += dTime;
		m_masterGoal.Process(dTime);
		m_Robot.Update(dTime);  //Now that we use events we need to process the robots time slices
		Wait(0.010);
	}
	//ensure we power off
	SetDrive(0.0, 0.0, m_activeCollection);
	SmartDashboard::PutBoolean("AutoPilot", false);
	//just do nothing, except give time back to CPU
	while (IsTest() && !IsDisabled())
	{
		m_Robot.Update(dTime);  //Now that we use events we need to process the robots time slices
		Wait(0.010);
	}
}

START_ROBOT_CLASS(Robot) //!< This identifies Robot as the main Robot starting class
