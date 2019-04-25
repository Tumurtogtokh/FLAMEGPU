/*
 * Copyright 2011 University of Sheffield.
 * Author: Dr Paul Richmond 
 * Contact: p.richmond@sheffield.ac.uk (http://www.paulrichmond.staff.shef.ac.uk)
 *
 * University of Sheffield retain all intellectual property and 
 * proprietary rights in and to this software and related documentation. 
 * Any use, reproduction, disclosure, or distribution of this software 
 * and related documentation without an express license agreement from
 * University of Sheffield is strictly prohibited.
 *
 * For terms of licence agreement please attached licence or view licence 
 * on www.flamegpu.com website.
 * 
 */
// includes, project
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "MenuDisplay.h"
#include "GlobalsController.h"
#include "CustomVisualisation.h"

#ifdef _MSC_VER
// Disable _CRT_SECURE_NO_WARNINGS warnings
#pragma warning(disable:4996)
#endif 

//holder for window width and height
int menu_width;
int menu_height;

int drawInfoDisplayState = 0;
int drawMenuDisplayState = 0;

//menu items

//info diaplay textoutput
int text_position = 0;

//external FLAME GPU functions from header.h
extern int get_agent_agent_MAX_count();
extern int get_agent_agent_default_count();
extern int get_agent_navmap_static_count();


//private prototypes
void updateAllProbabilityTexts(char* text);
void set2DProjection();
void printInfoLine(char *string);
void printMenuItem(menu_item* menu_item);


//menu items
menu_item* menu;
menu_item *em_rate1, *em_rate2;
menu_item *probability_1, *probability_2;
menu_item *time, *em_rate, *exit_state_1, *exit_state_2, *steer, *avoid, *collision, *goal;

void initMenuItems()
{

	time = (menu_item*)malloc(sizeof(menu_item));
	time->selected = 1;
	time->increase = increaseTimeScaler;
	time->decrease = decreaseTimeScaler;
	time->updateText = setTimeScalerText;
	time->updateText(time->text);

	steer = (menu_item*)malloc(sizeof(menu_item));
	steer->selected = 0;
	steer->increase = increaseSteerWeight;
	steer->decrease = decreaseSteerWeight;
	steer->updateText = setSteerWeightText;
	steer->updateText(steer->text);

	avoid = (menu_item*)malloc(sizeof(menu_item));
	avoid->selected = 0;
	avoid->increase = increaseAvoidWeight;
	avoid->decrease = decreaseAvoidWeight;
	avoid->updateText = setAvoidWeightText;
	avoid->updateText(avoid->text);

	collision = (menu_item*)malloc(sizeof(menu_item));
	collision->selected = 0;
	collision->increase = increaseCollisionWeight;
	collision->decrease = decreaseCollisionWeight;
	collision->updateText = setCollisionWeightText;
	collision->updateText(collision->text);

	goal = (menu_item*)malloc(sizeof(menu_item));
	goal->selected = 0;
	goal->increase = increaseGoalWeight;
	goal->decrease = decreaseGoalWeight;
	goal->updateText = setGoalWeightText;
	goal->updateText(goal->text);

	em_rate = (menu_item*)malloc(sizeof(menu_item));
	em_rate->selected = 0;
	em_rate->increase = increaseGlobalEmmisionRate;
	em_rate->decrease = decreaseGlobalEmmisionRate;
	em_rate->updateText = updateAllEmmsionRatesTexts;
	sprintf(em_rate->text, "GLOBAL EMMISION RATE");

	em_rate1 = (menu_item*)malloc(sizeof(menu_item));
	em_rate1->selected = 0;
	em_rate1->increase = increaseEmmisionRateExit1;
	em_rate1->decrease = decreaseEmmisionRateExit1;
	em_rate1->updateText = setEmmisionRateExit1Text;
	em_rate1->updateText(em_rate1->text);

	em_rate2 = (menu_item*)malloc(sizeof(menu_item));
	em_rate2->selected = 0;
	em_rate2->increase = increaseEmmisionRateExit2;
	em_rate2->decrease = decreaseEmmisionRateExit2;
	em_rate2->updateText = setEmmisionRateExit2Text;
	em_rate2->updateText(em_rate2->text);


	probability_1 = (menu_item*)malloc(sizeof(menu_item));
	probability_1->selected = 0;
	probability_1->increase = increaseProbabilityExit1;
	probability_1->decrease = decreaseProbabilityExit1;
	probability_1->updateText = updateAllProbabilityTexts;

	probability_2 = (menu_item*)malloc(sizeof(menu_item));
	probability_2->selected = 0;
	probability_2->increase = increaseProbabilityExit2;
	probability_2->decrease = decreaseProbabilityExit2;
	probability_2->updateText = updateAllProbabilityTexts;
	
	updateAllProbabilityTexts("");

	exit_state_1 = (menu_item*)malloc(sizeof(menu_item));
	exit_state_1->selected = 0;
	exit_state_1->increase = toggleStateExit1;
	exit_state_1->decrease = toggleStateExit1;
	exit_state_1->updateText = setStateExit1Text;
	exit_state_1->updateText(exit_state_1->text);

	exit_state_2 = (menu_item*)malloc(sizeof(menu_item));
	exit_state_2->selected = 0;
	exit_state_2->increase = toggleStateExit2;
	exit_state_2->decrease = toggleStateExit2;
	exit_state_2->updateText = setStateExit2Text;
	exit_state_2->updateText(exit_state_2->text);

	//build linked list
	time->previous = exit_state_2;

	time->next = steer;

	steer->previous = time;
	steer->next = avoid;
	avoid->previous = steer;
	avoid->next = collision;
	collision->previous = avoid;
	collision->next = goal;
	goal->previous = collision;
	goal->next = em_rate;

	em_rate->previous = goal;
	em_rate->next = em_rate1;
	em_rate1->previous = em_rate;
	em_rate1->next = em_rate2;
	em_rate2->previous = probability_1;

	probability_1->previous = em_rate2;
	probability_1->next = probability_2;
	probability_2->previous = probability_1;
	probability_2->next = exit_state_1;

	exit_state_1->previous = probability_1;
	exit_state_1->next = exit_state_2;
	exit_state_2->previous = exit_state_1;
	exit_state_2->next = time;


	menu = time;

}


int menuDisplayed()
{
	return drawMenuDisplayState;
}

void handleUpKey()
{
	if (menu)
	{
		menu_item* next = menu;
		int selected = 0;

		while(!selected)
		{
			next = next->next;
			selected = next->selected;
		}

		next->selected = 0;
		next->previous->selected = 1;
	}
}

void handleDownKey()
{
	if (menu)
	{
		menu_item* next = menu;
		int selected = 0;

		while(!selected)
		{
			next = next->next;
			selected = next->selected;
		}

		next->selected = 0;
		next->next->selected = 1;
	}
}

void handleLeftKey()
{
	if (menu)
	{
		menu_item* selected_item = menu;
		int selected = 0;

		while(!selected)
		{
			selected_item = selected_item->next;
			selected = selected_item->selected;
		}

		selected_item->decrease();
		if (selected_item->updateText != NULL)
			selected_item->updateText(selected_item->text);
	}
}

void handleRightKey()
{
	if (menu)
	{
		menu_item* selected_item = menu;
		int selected = 0;

		while(!selected)
		{
			selected_item = selected_item->next;
			selected = selected_item->selected;
		}

		selected_item->increase();
		if (selected_item->updateText != NULL)
			selected_item->updateText(selected_item->text);
	}
}


void drawInfoDisplay(int width, int height)
{
	if (drawInfoDisplayState)
	{			
		//draw text info
		char output_buffer[256];

		menu_width = width;
		menu_height = height;
		
		set2DProjection();
		
		text_position = 0;

                char simInfoString[] = "********** Simulation Information **********"; 
                char simEndInfoString[] = "******** End Simulation Information ********"; 
                
		glColor3f(0.0, 0.0, 0.0);
		printInfoLine(simInfoString);

		sprintf(output_buffer,"Current Frames Per Second: %f", getFPS());
		printInfoLine(output_buffer);

		sprintf(output_buffer,"Current Pedestrian Agent Count: %i", get_agent_agent_default_count());
		printInfoLine(output_buffer);

		sprintf(output_buffer,"Maximum Pedestrian Agent Count: %i", get_agent_agent_MAX_count());
		printInfoLine(output_buffer);

		sprintf(output_buffer,"Navigation Map Grid Cells: %i", get_agent_navmap_static_count());
		printInfoLine(output_buffer);

		sprintf(output_buffer,"Emmission Rate: %f", getEmmisionRateExit1());
		printInfoLine(output_buffer);

		printInfoLine(simEndInfoString);
	}
}

void toggleInformationDisplayOnOff()
{
	drawInfoDisplayState = !drawInfoDisplayState;
}

void setInformationDisplayOnOff(int state)
{
	drawInfoDisplayState = state;
}

void drawMenuDisplay(int width, int height)
{
	if (drawMenuDisplayState)
	{			
		menu_item* next;

		menu_width = width;
		menu_height = height;
		
		set2DProjection();

		text_position = 0;

                char simMenueString[] = "********** Simulation Menu **********"; 
                char simEndMenueString[] = "******** End Simulation Menu ********"; 
                
		glColor3f(0.0f,0.0f,0.0f);
		printInfoLine(simMenueString);

		//print menu
		if (menu)
		{
			printMenuItem(menu);

			next = menu->next;

			while(next != menu)
			{
				printMenuItem(next);
				next = next->next;
			}
		}

		glColor3f(0.0f,0.0f,0.0f);
		printInfoLine(simEndMenueString);
	}
}

void toggleMenuDisplayOnOff()
{
	drawMenuDisplayState = !drawMenuDisplayState;
}

void setMenuDisplayOnOff(int state)
{
	drawMenuDisplayState = state;
}

void updateAllTexts(){
	// Iterate the linked list updating menu item texts, until we get to the start again.
	menu_item* nextItem = menu;
	do{
		nextItem->updateText(nextItem->text);
		nextItem = nextItem->next;
	} while(nextItem != menu);
}

void updateAllEmmsionRatesTexts(char* text)
{
	em_rate1->updateText(em_rate1->text);
	em_rate2->updateText(em_rate2->text);
}

void updateAllProbabilityTexts(char* text){
	setProbabilityExit1Text(probability_1->text);
	setProbabilityExit2Text(probability_2->text);
}

void set2DProjection()
{
	//set projection mode for rendering text
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, menu_width, menu_height, 0);
	glScalef(1, -1, 1);
	glTranslatef(0, -menu_height, 0);
	glColor3f(1.0, 1.0, 1.0);

	glDisable( GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
}

void printInfoLine(char *string)
{
 
  char *c;
  glRasterPos2f(10.0f, (menu_height - 20) - text_position);
  for (c=string; *c != '\0'; c++) {
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *c);
  }
  text_position += 20.0f;
}

void printMenuItem(menu_item* menu_item)
{
 
	if (menu_item->selected)
		  glColor3f(1.0f,0.0f,0.0f);
	else
		  glColor3f(0.0f,0.0f,0.0f);

	printInfoLine(menu_item->text);
}
