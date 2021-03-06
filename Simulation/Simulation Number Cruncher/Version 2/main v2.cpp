/*
  Programmer: Lucas Winkelman & Tanner Winkelman
  File: main.cpp
  Purpose: To simulate matches with a varying drone heuristic
  Warning!  This program uses global variables.
*/

#include <windows.h>
#include <gdiplus.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
using namespace Gdiplus;
using namespace std;

// The starting score of a match
const float MATCH_START_SCORE = 12000;

// The points to add to a match per robot scored
const float ROBOT_SCORED_POINTS = 2000;

// The points to add to a match per robot not scored
const float ROBOT_NOT_SCORED_POINTS = -1000;

// The points to add to a match per minute used in the match
const float MINUTE_USED_POINTS = -100;

// Frames per second when replaying
const float DISPLAY_FRAME_RATE = 30;

// Frames per second when computing matches
const float COMP_FRAME_RATE = 15;

// The width and height in pixels of the arena (the drawing space for the field, but not the field)
const float ARENA_WIDTH = 650;
const float ARENA_HEIGHT = 650;

// A gdiplus color object for the outline of the arena
const Color ARENA_EDGE_COLOR = Color(255, 200, 200, 200);

// The space in the window to the left and top of the arena
const int ARENA_LEFT_MARGIN = 10;
const int ARENA_TOP_MARGIN = 10;

// The number of ground robots, obstacle robots, and drones
const int GROUND_ROBOT_NUMBER = 10;
const int OBSTACLE_ROBOT_NUMBER =  4;
const int DRONE_NUMBER = 1;

// The length in minutes of a match
const int MATCH_LENGTH = 10;

const int SECONDS_IN_MINUTE = 60;

// Number of margins around the field
const int FIELD_MARGIN_COUNT = 2;

// The size in meters of each margin
const int FIELD_MARGIN_SIZE = 2;

// The size of the field in meters
const int FIELD_METER_WIDTH = 20;

const int CENTI_IN_METER = 100;

// The size of the field's tape in centimeters
const float TAPE_CM_WIDTH = 7;

const float TO_RADIANS = 0.0174533;
const float TO_DEGREES = 57.29580;
const float DEGREES_IN_CIRCLE = 360;
const float PI = 3.14159265359;

const float METER_PIXEL_DIST = ARENA_HEIGHT / (FIELD_METER_WIDTH + (FIELD_MARGIN_SIZE * FIELD_MARGIN_COUNT));

// Height and width in pixels of the field
const float FIELD_HEIGHT = (METER_PIXEL_DIST * FIELD_METER_WIDTH);
const float FIELD_WIDTH = FIELD_HEIGHT;

// X and Y pixel position of the field in the arena
const float FIELD_X = (ARENA_WIDTH - FIELD_WIDTH) / 2;
const float FIELD_Y = (ARENA_HEIGHT - FIELD_HEIGHT) / 2;

// The field's tape's width in pixels
const float TAPE_WIDTH = TAPE_CM_WIDTH / CENTI_IN_METER * METER_PIXEL_DIST;

// The obstacle robot's difference in rotation from its rotation from with the center of the field
const float OBSTACLE_ROBOT_TURN_AMOUNT = 90;

// The radius of an obstacle robot's path around the center
const float OBSTACLE_ROBOT_PATH_RADIUS = 5;

// The drone's diameter in meters
const float DRONE_DIA = 0.3;

// The drone's argb color
const Color DRONE_COLOR = Color(255, 255, 255, 0);

// The field's tape's argb color
const Color WHITE_TAPE_COLOR = Color(255, 255, 255, 255);
const Color GREEN_TAPE_COLOR = Color(255, 0, 255, 0);
const Color RED_TAPE_COLOR = Color(255, 255, 0, 0);

// The ground robots' color
const Color GROUND_ROBOT_COLOR = Color(255, 255, 255, 255);

// The ground robots' diameter in meters
const float GROUND_ROBOT_DIA = 0.353;

// The obstacle robots' diameter in meters
const float OBSTACLE_ROBOT_DIA = 0.353;

// The ground robots' color
const Color OBSTACLE_ROBOT_COLOR = Color(255, 200, 200, 200);

// The arena's background color
const Color ARENA_BACKGROUND_COLOR = Color(255, 150, 150, 150);

// The color red ground robots' direction pointers
const Color RED_ROBOT_COLOR = Color(255, 255, 0, 0);

// The color blue ground robots' direction pointers
const Color BLUE_ROBOT_COLOR = Color(255, 0, 0, 255);

// The direction pointer's length
const float DIRECTION_POINTER_LENGTH = 1;

// The ground robots' speed in m/s
const float GROUND_ROBOT_SPEED = 0.33;

// The obstacle robots' speed in m/s
const float OBSTACLE_ROBOT_SPEED = 0.33;

// The time it takes an obstacle robot to do a full circle in seconds
const float OBSTACLE_ROBOT_FULL_CIRLE_TIME = 2 * OBSTACLE_ROBOT_PATH_RADIUS * PI / OBSTACLE_ROBOT_SPEED;

// The obstacle robots' speed in pixels
const float OBSTACLE_ROBOT_PIXEL_SPEED = OBSTACLE_ROBOT_SPEED * FIELD_HEIGHT / FIELD_METER_WIDTH / COMP_FRAME_RATE;

// The amount of time of a match in seconds
const float TIME_LIMIT = SECONDS_IN_MINUTE * MATCH_LENGTH;

// The number of frames in one match
const int FRAME_LIMIT = COMP_FRAME_RATE * TIME_LIMIT;

// The ground robots' speed in pixels
const float GROUND_ROBOT_PIXEL_SPEED = GROUND_ROBOT_SPEED * FIELD_HEIGHT / FIELD_METER_WIDTH / COMP_FRAME_RATE;

// The numbers for the various turning modes
const int FORWARD_MODE = 0;
const int REVERSE_MODE = 1;
const int NOISE_MODE = 2;
const int COLLISION_MODE = 3;
const int TAP_MODE = 4;
const int TOP_TOUCH_MODE = 5;

// The rotation in degrees of reverse and noise modes
const float REVERSE_DURATION = 2.150;
const float NOISE_MAX_ROTATION = 20;
const float NOISE_MIN_ROTATION = 0;

// The interval time between each reverse and noise action
const float REVERSE_INTERVAL = 20;
const float NOISE_INTERVAL = 5;

// The degrees turned when the robot reverses
const float REVERSE_DEGREES = 180;

// The degrees turned when the robot collides
const float COLLISION_DEGREES = 180;

// The degrees turned when a ground robot gets touched on top
const float TOP_TOUCH_DEGREES = 45;

// The speed the robot turns at in degrees per second
const float GROUND_ROBOT_TURN_SPEED = REVERSE_DEGREES / REVERSE_DURATION;

// IDs for the two ground robot colors
const int RED_ID = 0;
const int BLUE_ID = 1;

// The drone's acceleration
const float DRONE_ACCEL = 1.3 / COMP_FRAME_RATE;

// The min and max of the range of multipliers that get multiplied with the distance
//   of a roomba from the green line
const float GREEN_LINE_MULT_MIN = -5;
const float GREEN_LINE_MULT_MAX = 5;

// The min and max of the range of multipliers that get multiplied with the closest distance to an edge
//   except for the green one
const float DIST_FROM_EDGE_MIN = -5;
const float DIST_FROM_EDGE_MAX = 5;

// The min and max of the distance from drone multiplier
const float DIST_FROM_DRONE_MIN = -5;
const float DIST_FROM_DRONE_MAX = 5;

// The min and max of the
const float HEAD_OFF_EDGE_MIN = -5;
const float HEAD_OFF_EDGE_MAX = 5;

// The maximum distance the drone can from right above the ground robot to top touch it (in meters)
const float TOP_TOUCH_MAX_OFFSET = 0.2;

// The number of matches to run per data set
const int MATCHES_PER_SET = 25;

// The number of data sets
const int DATA_SET_COUNT = 20;

// The maximum speed difference between the drone and the ground robot it wants to touch
const float TOP_TOUCH_MAX_SPEED_DIFF = 0.2;

// The minimum safe distance from an obstacle robot
const float DRONE_FROM_OBS_MIN_SAFE_DIST = 1;


struct Ground_Robot_Data
{
    float X;
    float Y;
    float Rotation;
    int Color;
    int Frames_To_Turn;
    int Turn_Direction_Mult;
    bool Still_In;
    bool Scored;
    char Mode;
    int Last_Frame_Reversed;
    int Last_Frame_Noise;
    int Time_Left_To_Rotate;
};

struct Obstacle_Robot_Data
{
    float X;
    float Y;
    float Rotation;
};

struct Drone_Data
{
    float X;
    float Y;
    float X_Speed;
    float Y_Speed;
    int Target_Robot;
    bool Ok_To_Leave;
};

struct Game_Frame
{
    vector<Ground_Robot_Data> Ground_Robots;
    vector<Obstacle_Robot_Data> Obstacle_Robots;
    vector<Drone_Data> Drones;
};

struct Result
{
    float Ground_Robots_Scored;
    float Time_Taken;
    float Score;
};

struct Data_Set
{
    float Green_Line_Mult;
    float Dist_From_Edge_Mult;
    float Dist_From_Drone_Mult;
    vector<Result> Results;
    float Ave_Robots;
    float Ave_Time_Taken;
    float Ave_Score;
};


// Desc: Initializes the first frame of a match
// Pre: Relies on global variables, especially Array_Of_Frames
// Post: The first frame is set up and we are ready to run the match (Array_Of_Frames[0] is set up)
void Init();

// Desc: The function that get called when the window gets a Windows Message.
//   This keeps the window responsive
// Pre: None (that we care about, the OS takes care of calling it the right way)
// Post: An action may be taken inside the function that may effect global variables or perform
//   an action on the window
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Desc: This function draws each from on the screen
// Pre: hWnd must be a handle to our main window, the global variable Current_Frame is the frame drawn
// Post: The frame is drawn to the screen
void DrawScreen(HWND hWnd);

// Desc: This function makes a DIB section that can be attached to a Device context
// Pre: hdc must be a Handle to the Device Context we want this bitmap to be compatible with
// Post: returns a Handle to a BITMAP that is compatible with the Device Context given and with the dimensions w and h
HBITMAP CreateDIBSectionFunction(HDC hdc, int W,  int H);

// Desc: This function runs a frame of the game and changes variables
// Pre: Array_Of_Frames and Current_Frame must exist
// Post: the values in Array_Of_Frames are changed
void Engine();

// Desc: This function initializes the multipliers in the data sets
// Pre: None
// Post: The multipliers in Data_Sets are initialized
void Init_Mults();

// Desc: Scores match at Current_Frame in Array_Of_Frames
// Pre: None
// Post: Fills in Data_Sets[Current_Data_Set].Results[Current_Match_In_Set] with the scoring data from the match
void Score_Match();

// Desc: Calculate the average of a data set's matches's scores
// Pre: None
// Post: Calculate the averages for Data_Set[Current_Data_Set] and sets its variables
void Calc_Set_Ave();

// Desc: Print out a set on the screen
// Pre: index must be positive
// Post: Data_Sets[index] will be outputted
void Output_Set(int index);

// Desc: Data_Sets by each items score by bubble sort
// Pre: None
// Post: Data_Sets will be sorted
void Sort_Sets_By_Score();



void Make_New_Sets();



void Gen_Random_Set(int index);



// We are pretty much forced to use global variables with the way a windows c++ program is set up :(
vector<Game_Frame> Array_Of_Frames(FRAME_LIMIT + 1);
vector<Data_Set> Data_Sets(DATA_SET_COUNT);
int Current_Frame = 0;
int Current_Data_Set = 0;
int Current_Match_In_Set = 0;
int Frame_Ended = FRAME_LIMIT;
bool Has_Ended = false;

float firex = 0;
float firey = 0;

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, INT iCmdShow)
{
    HWND                hWnd;
    MSG                 msg;
    WNDCLASS            wndClass;
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR           gdiplusToken;
    HDC  WindowHDC;
    RECT Window_Size_Rect;
    float Milli_From_Last_Frame = 0;

    srand(time(NULL));

    // Initialize GDI+.
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    wndClass.style          = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc    = WndProc;
    wndClass.cbClsExtra     = 0;
    wndClass.cbWndExtra     = 0;
    wndClass.hInstance      = hInstance;
    wndClass.hIcon          = LoadIcon(NULL, IDI_APPLICATION);
    wndClass.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wndClass.hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndClass.lpszMenuName   = NULL;
    wndClass.lpszClassName  = TEXT("GettingStarted");

    RegisterClass(&wndClass);

    hWnd = CreateWindow(
        TEXT("GettingStarted"),   // window class name
        TEXT("Getting Started"),  // window caption
        WS_OVERLAPPEDWINDOW,      // window style
        CW_USEDEFAULT,            // initial x position
        CW_USEDEFAULT,            // initial y position
        CW_USEDEFAULT,            // initial x size
        CW_USEDEFAULT,            // initial y size
        NULL,                     // parent window handle
        NULL,                     // window menu handle
        hInstance,                // program instance handle
        NULL);                    // creation parameters

    ShowWindow(hWnd, iCmdShow);

    WindowHDC = GetDC(hWnd);

    cout << "before init" << endl;
    Init();
    Init_Mults();
    cout << "made it 1";

    bool Winner_Shown = false;
    // The message loop
    while(TRUE)
    {

        while(Current_Data_Set < DATA_SET_COUNT)
        {
            while(Current_Match_In_Set < MATCHES_PER_SET)
            {
                while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
                if(msg.message == WM_QUIT)
                    break;


                Frame_Ended = FRAME_LIMIT;
                Has_Ended = false;
                while(Current_Frame < FRAME_LIMIT)
                {
                    Current_Frame++;
                    Engine();
                    //DrawScreen(hWnd);
                }
                // End of match
                Score_Match();
                Current_Frame = 0;
                Init();
                Current_Match_In_Set++;
            }
            // End of data set
            Current_Match_In_Set = 0;
            Calc_Set_Ave();
            Output_Set(Current_Data_Set);

            Current_Data_Set++;
        }

        Current_Data_Set = 0;
        Sort_Sets_By_Score();
        Make_New_Sets();
        /*
        if(Winner_Shown == false)
        {
            int Best_Set = 0;
            float High_Score = 0;
            for(int counter = 0; counter < DATA_SET_COUNT; counter++)
            {
                if(Data_Sets[counter].Ave_Score > High_Score)
                {
                    High_Score = Data_Sets[counter].Ave_Score;
                    Best_Set = counter;
                }
            }

            Winner_Shown = true;
            Output_Set(Best_Set);
        }
        */
    }

   GdiplusShutdown(gdiplusToken);
   return msg.wParam;
}  // WinMain


// This function gets called in response to messages to the window
// It gets called after the DispatchMessage function in the message loop
LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
   WPARAM wParam, LPARAM lParam)
{
    HDC          WindowHDC;
    PAINTSTRUCT  ps;

    switch(message)
    {
        // If the window needs to be repainted
        case WM_PAINT:
        {
            WindowHDC = BeginPaint(hWnd, &ps);
            DrawScreen(hWnd);
            EndPaint(hWnd, &ps);
            break;
        }

        // If the window's close (X) button has been clicked
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            break;
        }

        // Otherwise do the default
        default:
        {
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    return 0;
}



void DrawScreen(HWND hWnd)
{
    RECT Window_Size_Rect;
    HDC Window_HDC;

    // Get the window's width and height
    GetWindowRect(hWnd, &Window_Size_Rect);
    int Window_Width = Window_Size_Rect.right;
    int Window_Height = Window_Size_Rect.bottom;

    // Get a Handle to the Device Context of hWnd
    Window_HDC = GetDC(hWnd);

    //***** GDI and GDI+ stuff to get ready for drawing *****//
    HDC Back_Buffer_HDC = CreateCompatibleDC(Window_HDC);
    HBITMAP Back_Buffer_HBITMAP = CreateDIBSectionFunction(Back_Buffer_HDC, Window_Width, Window_Height);
    HBITMAP Original_Back_Buffer_HBITMAP = (HBITMAP)SelectObject(Back_Buffer_HDC, Back_Buffer_HBITMAP);

    // This is the object that "draws" stuff for us
    Graphics Back_Buffer_Drawer(Back_Buffer_HDC);


    //****** Now ready to draw *****//
    // Clear the back buffer to white
    Back_Buffer_Drawer.Clear(Color(255, 255, 255, 255));

    //***** Create the "arena" and its parts *****//
    HDC Arena_HDC = CreateCompatibleDC(Back_Buffer_HDC);
    HBITMAP Arena_HBITMAP = CreateDIBSectionFunction(Arena_HDC, ARENA_WIDTH, ARENA_HEIGHT);
    HBITMAP Original_Arena_HBITMAP = (HBITMAP)SelectObject(Arena_HDC, Arena_HBITMAP);
    Graphics Arena_Drawer(Arena_HDC);

    // Turn on anti-aliasing for Arena_Drawer
    Arena_Drawer.SetSmoothingMode(SmoothingModeAntiAlias);

    // Clear to white
    Arena_Drawer.Clear(ARENA_BACKGROUND_COLOR);

    // Draw an outline around the arena
    Pen Arena_Edge_Pen(ARENA_EDGE_COLOR);
    Arena_Drawer.DrawRectangle(&Arena_Edge_Pen, 0, 0, static_cast<int>(ARENA_WIDTH - 1), static_cast<int>(ARENA_HEIGHT - 1));

    // Draw the horizontal pieces tape
    Pen White_Tape_Pen(WHITE_TAPE_COLOR, TAPE_WIDTH);
    for(int index = 1; index < FIELD_METER_WIDTH; index++)
    {
        Arena_Drawer.DrawLine(&White_Tape_Pen, FIELD_X, FIELD_Y + (METER_PIXEL_DIST * index), FIELD_X + FIELD_WIDTH, FIELD_Y + (METER_PIXEL_DIST * index));
    }

    // Draw the vertical pieces of tape
    for(int index = 0; index < FIELD_METER_WIDTH + 1; index++)
    {
        Arena_Drawer.DrawLine(&White_Tape_Pen, FIELD_X + (METER_PIXEL_DIST * index), FIELD_Y, FIELD_X + (METER_PIXEL_DIST * index), FIELD_Y + FIELD_HEIGHT);
    }

    // Create red and green pens
    Pen Red_Tape_Pen(RED_TAPE_COLOR, TAPE_WIDTH);
    Pen Green_Tape_Pen(GREEN_TAPE_COLOR, TAPE_WIDTH);

    // Draw a red and a green line
    Arena_Drawer.DrawLine(&Red_Tape_Pen, FIELD_X, FIELD_Y + FIELD_HEIGHT, FIELD_X + FIELD_WIDTH, FIELD_Y + FIELD_HEIGHT);
    Arena_Drawer.DrawLine(&Green_Tape_Pen, FIELD_X, FIELD_Y, FIELD_X + FIELD_WIDTH, FIELD_Y);

    // Draw the ground robots
    SolidBrush Ground_Robot_Brush(GROUND_ROBOT_COLOR);
    Pen Red_Robot_Pen(RED_ROBOT_COLOR);
    Pen Blue_Robot_Pen(BLUE_ROBOT_COLOR);
    for(int index = 0; index < GROUND_ROBOT_NUMBER; index++)
    {
        float Temp_X = Array_Of_Frames[Current_Frame].Ground_Robots[index].X;
        float Temp_Y = Array_Of_Frames[Current_Frame].Ground_Robots[index].Y;
        Arena_Drawer.FillEllipse(&Ground_Robot_Brush, Array_Of_Frames[Current_Frame].Ground_Robots[index].X - ((GROUND_ROBOT_DIA * METER_PIXEL_DIST) / 2), Array_Of_Frames[Current_Frame].Ground_Robots[index].Y - ((GROUND_ROBOT_DIA * METER_PIXEL_DIST) / 2),
            GROUND_ROBOT_DIA * METER_PIXEL_DIST, GROUND_ROBOT_DIA * METER_PIXEL_DIST);

        // Draw direction pointer on robot
        if(Array_Of_Frames[Current_Frame].Ground_Robots[index].Color == RED_ID)
        {
            Arena_Drawer.DrawLine(&Red_Robot_Pen, Temp_X, Temp_Y,
                Temp_X + sin(Array_Of_Frames[Current_Frame].Ground_Robots[index].Rotation * TO_RADIANS) * DIRECTION_POINTER_LENGTH * METER_PIXEL_DIST,
                Temp_Y + cos(Array_Of_Frames[Current_Frame].Ground_Robots[index].Rotation * TO_RADIANS) * DIRECTION_POINTER_LENGTH * METER_PIXEL_DIST);
        }
        else
        {
            Arena_Drawer.DrawLine(&Blue_Robot_Pen, Temp_X, Temp_Y,
                Temp_X + sin(Array_Of_Frames[Current_Frame].Ground_Robots[index].Rotation * TO_RADIANS) * DIRECTION_POINTER_LENGTH * METER_PIXEL_DIST,
                Temp_Y + cos(Array_Of_Frames[Current_Frame].Ground_Robots[index].Rotation * TO_RADIANS) * DIRECTION_POINTER_LENGTH * METER_PIXEL_DIST);
        }
    }

    // Draw obstacle robots
    SolidBrush Obstacle_Robot_Brush(OBSTACLE_ROBOT_COLOR);
    Pen Obstacle_Robot_Pen(OBSTACLE_ROBOT_COLOR);
    for(int index = 0; index < OBSTACLE_ROBOT_NUMBER; index++)
    {
        float Temp_X = Array_Of_Frames[Current_Frame].Obstacle_Robots[index].X;
        float Temp_Y = Array_Of_Frames[Current_Frame].Obstacle_Robots[index].Y;
        Arena_Drawer.FillEllipse(&Obstacle_Robot_Brush, Array_Of_Frames[Current_Frame].Obstacle_Robots[index].X - ((OBSTACLE_ROBOT_DIA * METER_PIXEL_DIST) / 2), Array_Of_Frames[Current_Frame].Obstacle_Robots[index].Y - ((OBSTACLE_ROBOT_DIA * METER_PIXEL_DIST) / 2),
            OBSTACLE_ROBOT_DIA * METER_PIXEL_DIST, OBSTACLE_ROBOT_DIA * METER_PIXEL_DIST);
        Arena_Drawer.DrawLine(&Obstacle_Robot_Pen, Temp_X, Temp_Y,
            Temp_X + sin(Array_Of_Frames[Current_Frame].Obstacle_Robots[index].Rotation * TO_RADIANS) * DIRECTION_POINTER_LENGTH * METER_PIXEL_DIST,
            Temp_Y + cos(Array_Of_Frames[Current_Frame].Obstacle_Robots[index].Rotation * TO_RADIANS) * DIRECTION_POINTER_LENGTH * METER_PIXEL_DIST);
    }


    // Draw the drone
    SolidBrush Drone_Brush(DRONE_COLOR);
    Arena_Drawer.FillEllipse(&Drone_Brush, Array_Of_Frames[Current_Frame].Drones[0].X - ((DRONE_DIA * METER_PIXEL_DIST) / 2), Array_Of_Frames[Current_Frame].Drones[0].Y - ((DRONE_DIA * METER_PIXEL_DIST) / 2),
        DRONE_DIA * METER_PIXEL_DIST, DRONE_DIA * METER_PIXEL_DIST);
    Arena_Drawer.FillEllipse(&Drone_Brush, firex - ((DRONE_DIA * METER_PIXEL_DIST) / 2), firey - ((DRONE_DIA * METER_PIXEL_DIST) / 2),
        DRONE_DIA * METER_PIXEL_DIST, DRONE_DIA * METER_PIXEL_DIST);

    // Draw arena onto the back buffer
    BitBlt(Back_Buffer_HDC, ARENA_LEFT_MARGIN, ARENA_TOP_MARGIN, Window_Width, Window_Height, Arena_HDC, 0, 0, SRCCOPY);
    //cout << "fire2" << endl;
    DeleteDC(Arena_HDC);
    DeleteObject(Arena_HBITMAP);


	LinearGradientBrush LGB(Point(0,0), Point(0,500), Color(255,0,0,255), Color(255,0,0,200));
	//int ok = Back_Buffer_Drawer.FillRectangle(&LGB, 0, 0, 400, 500);

	// Copy the back buffer to the window
	BitBlt(Window_HDC, 0, 0, Window_Width, Window_Height, Back_Buffer_HDC, 0, 0, SRCCOPY);

	// This is supposed to be done before at the end of drawing
	SelectObject(Back_Buffer_HDC, Original_Back_Buffer_HBITMAP);
	DeleteDC(Back_Buffer_HDC);
	DeleteObject(Back_Buffer_HBITMAP);
}



void Engine()
{
    Ground_Robot_Data * Current_G_Robot;
    Drone_Data * Current_Drone;
    int Robot_High_Score = -99999;
    int Robot_With_High_Score = 0;

    //*** Copy data from last frame ***//
    for(int index = 0; index < DRONE_NUMBER; index++)
    {

        Array_Of_Frames[Current_Frame].Drones[index].X_Speed = Array_Of_Frames[Current_Frame - 1].Drones[index].X_Speed;
        Array_Of_Frames[Current_Frame].Drones[index].Y_Speed = Array_Of_Frames[Current_Frame - 1].Drones[index].Y_Speed;
        Array_Of_Frames[Current_Frame].Drones[index].X = Array_Of_Frames[Current_Frame - 1].Drones[index].X;
        Array_Of_Frames[Current_Frame].Drones[index].Y = Array_Of_Frames[Current_Frame - 1].Drones[index].Y;
        Array_Of_Frames[Current_Frame].Drones[index].Target_Robot = Array_Of_Frames[Current_Frame - 1].Drones[index].Target_Robot;
        Array_Of_Frames[Current_Frame].Drones[index].Ok_To_Leave = Array_Of_Frames[Current_Frame - 1].Drones[index].Ok_To_Leave;

    }


    for(int index = 0; index < GROUND_ROBOT_NUMBER; index++)
    {

        Array_Of_Frames[Current_Frame].Ground_Robots[index].Rotation = Array_Of_Frames[Current_Frame - 1].Ground_Robots[index].Rotation;
        Array_Of_Frames[Current_Frame].Ground_Robots[index].X = Array_Of_Frames[Current_Frame - 1].Ground_Robots[index].X;
        Array_Of_Frames[Current_Frame].Ground_Robots[index].Y = Array_Of_Frames[Current_Frame - 1].Ground_Robots[index].Y;
        Array_Of_Frames[Current_Frame].Ground_Robots[index].Color = Array_Of_Frames[Current_Frame - 1].Ground_Robots[index].Color;
        Array_Of_Frames[Current_Frame].Ground_Robots[index].Frames_To_Turn = Array_Of_Frames[Current_Frame - 1].Ground_Robots[index].Frames_To_Turn;
        Array_Of_Frames[Current_Frame].Ground_Robots[index].Turn_Direction_Mult = Array_Of_Frames[Current_Frame - 1].Ground_Robots[index].Turn_Direction_Mult;
        Array_Of_Frames[Current_Frame].Ground_Robots[index].Still_In = Array_Of_Frames[Current_Frame - 1].Ground_Robots[index].Still_In;
        Array_Of_Frames[Current_Frame].Ground_Robots[index].Scored = Array_Of_Frames[Current_Frame - 1].Ground_Robots[index].Scored;
        Array_Of_Frames[Current_Frame].Ground_Robots[index].Last_Frame_Reversed = Array_Of_Frames[Current_Frame - 1].Ground_Robots[index].Last_Frame_Reversed;
        Array_Of_Frames[Current_Frame].Ground_Robots[index].Last_Frame_Noise = Array_Of_Frames[Current_Frame - 1].Ground_Robots[index].Last_Frame_Noise;
        Array_Of_Frames[Current_Frame].Ground_Robots[index].Time_Left_To_Rotate = Array_Of_Frames[Current_Frame - 1].Ground_Robots[index].Time_Left_To_Rotate;
        Array_Of_Frames[Current_Frame].Ground_Robots[index].Mode = Array_Of_Frames[Current_Frame - 1].Ground_Robots[index].Mode;

    }


    for(int index = 0; index < OBSTACLE_ROBOT_NUMBER; index++)
    {
        Array_Of_Frames[Current_Frame].Obstacle_Robots[index].Rotation = Array_Of_Frames[Current_Frame - 1].Obstacle_Robots[index].Rotation;
        Array_Of_Frames[Current_Frame].Obstacle_Robots[index].X = Array_Of_Frames[Current_Frame - 1].Obstacle_Robots[index].X;
        Array_Of_Frames[Current_Frame].Obstacle_Robots[index].Y = Array_Of_Frames[Current_Frame - 1].Obstacle_Robots[index].Y;
    }

    //*** Now for changing data ***//
    for(int index1 = 0; index1 < DRONE_NUMBER; index1++)
    {
        Current_Drone = &Array_Of_Frames[Current_Frame].Drones[index1];
        for(int index = 0; index < GROUND_ROBOT_NUMBER; index++)
        {
            Ground_Robot_Data * Current_G_Robot;
            Current_G_Robot = &Array_Of_Frames[Current_Frame].Ground_Robots[index];
            if(Current_G_Robot->Still_In == true)
            {
                // Variable to hold this robot's score
                float Temp_Score = 0;

                // Add (dist from green line * multiplier)
                Temp_Score = Temp_Score + (abs(Current_G_Robot->Y - FIELD_Y) / METER_PIXEL_DIST * Data_Sets[Current_Data_Set].Green_Line_Mult);

                // Determine dist in meters from closest edge (other than green) the multiply then add
                float Dist_To_Closest_Edge = 0;
                float From_Left_Dist = abs(Current_G_Robot->X - FIELD_X);
                float From_Right_Dist = abs((FIELD_X + FIELD_WIDTH) - Current_G_Robot->X);
                float From_Bottom_Dist = abs((FIELD_Y + FIELD_HEIGHT) - Current_G_Robot->Y);
                if(From_Left_Dist > From_Right_Dist)
                {
                    if(From_Right_Dist > From_Bottom_Dist)
                    {
                        Dist_To_Closest_Edge = From_Bottom_Dist;
                    }
                    else
                    {
                        Dist_To_Closest_Edge = From_Right_Dist;
                    }
                }
                else
                {
                    if(From_Left_Dist > From_Bottom_Dist)
                    {
                        Dist_To_Closest_Edge = From_Bottom_Dist;
                    }
                    else
                    {
                        Dist_To_Closest_Edge = From_Left_Dist;
                    }
                }
                Temp_Score = Temp_Score + (Dist_To_Closest_Edge * Data_Sets[Current_Data_Set].Dist_From_Edge_Mult);

                // Determine dist from drone, multiply and add to Temp_Score
                float A = Current_Drone->X - Current_G_Robot->X;
                float B = Current_Drone->Y - Current_G_Robot->Y;
                float Dist = sqrt(A * A + B * B) * METER_PIXEL_DIST;
                Temp_Score = Temp_Score + (Dist * Data_Sets[Current_Data_Set].Dist_From_Drone_Mult);

                // If the ground robot has a new high score
                if(Temp_Score > Robot_High_Score)
                {
                    Robot_High_Score = Temp_Score;
                    Robot_With_High_Score = index;
                }
            }
        }

        // Compute ahead to see if the drone is going too fast
        Ground_Robot_Data * Target_G_Robot;
        Target_G_Robot = &Array_Of_Frames[Current_Frame].Ground_Robots[Robot_With_High_Score];

        bool Slow_Down = false;
        float Temp_XSpeed = Current_Drone->X_Speed;
        float Temp_YSpeed = Current_Drone->Y_Speed;
        float Temp_X = Current_Drone->X + Current_Drone->X_Speed;
        float Temp_Y = Current_Drone->Y + Current_Drone->Y_Speed;
        while(Temp_XSpeed != 0 || Temp_YSpeed != 0)
        {
            Temp_X = Temp_X + Temp_XSpeed;
            Temp_Y = Temp_Y + Temp_YSpeed;

            float Temp_X_Accel = 0;
            float Temp_Y_Accel = 0;

            if(Temp_XSpeed > DRONE_ACCEL)
            {
                Temp_X_Accel = -DRONE_ACCEL;
            }
            else if(Temp_XSpeed < -DRONE_ACCEL)
            {
              Temp_X_Accel = DRONE_ACCEL;
            }
            else
            {
              Temp_XSpeed = 0;
            }

            if(Temp_YSpeed > DRONE_ACCEL)
            {
              Temp_Y_Accel = -DRONE_ACCEL;
            }
            else if(Temp_YSpeed < -DRONE_ACCEL)
            {
              Temp_Y_Accel = DRONE_ACCEL;
            }
            else
            {
              Temp_YSpeed = 0;
            }

            float Movement_Dist = sqrt(Temp_X_Accel * Temp_X_Accel + Temp_Y_Accel * Temp_Y_Accel);
            if(Movement_Dist > DRONE_ACCEL)
            {
                Temp_X_Accel = Temp_X_Accel / (Movement_Dist / DRONE_ACCEL);
                Temp_Y_Accel = Temp_Y_Accel / (Movement_Dist / DRONE_ACCEL);
            }

            Temp_XSpeed = Temp_XSpeed + Temp_X_Accel;
            Temp_YSpeed = Temp_YSpeed + Temp_Y_Accel;
        }

        float Temp_Drone_Prev_X_Speed = Current_Drone->X_Speed;
        float Temp_Drone_Prev_Y_Speed = Current_Drone->Y_Speed;

        float Temp_X_Accel = 0;
        float Temp_Y_Accel = 0;

        // Check to see what obstacle robot is closest to the drone
        float Closest_Obs_Robot_Dist = 9999999;
        float Closest_Obs_Robot = 0;
        for(int counter = 0; counter < OBSTACLE_ROBOT_NUMBER; counter++)
        {
            float A = Current_Drone->X - Array_Of_Frames[Current_Frame].Obstacle_Robots[counter].X;
            float B = Current_Drone->Y - Array_Of_Frames[Current_Frame].Obstacle_Robots[counter].Y;
            float Temp_Dist = sqrt(A * A + B * B);

            if(Temp_Dist < Closest_Obs_Robot_Dist)
            {
                Closest_Obs_Robot_Dist = Temp_Dist;
                Closest_Obs_Robot = counter;
            }
        }

        // If too close move away
        float min_dist = (DRONE_DIA * METER_PIXEL_DIST / 2 + OBSTACLE_ROBOT_DIA * METER_PIXEL_DIST / 2) + DRONE_FROM_OBS_MIN_SAFE_DIST * METER_PIXEL_DIST;
        if(Closest_Obs_Robot_Dist < min_dist)
        {
            float Rotation_To_Closest_Obs = TO_DEGREES * atan2(Current_Drone->Y - Array_Of_Frames[Current_Frame].Obstacle_Robots[Closest_Obs_Robot].Y,
                                   Current_Drone->X - Array_Of_Frames[Current_Frame].Obstacle_Robots[Closest_Obs_Robot].X);
            Temp_X_Accel =  sin((Rotation_To_Closest_Obs + 180) * TO_RADIANS) * DRONE_ACCEL;
            Temp_Y_Accel = cos((Rotation_To_Closest_Obs + 180) * TO_RADIANS) * DRONE_ACCEL;
        }

        // Else move toward target ground robot
        else
        {
            //cout << Temp_XSpeed << " " << Current_Drone->X_Speed << endl;
            if(Temp_X > Target_G_Robot->X)
            {
                Temp_X_Accel = -DRONE_ACCEL;
            }
            else
            {
                Temp_X_Accel = DRONE_ACCEL;
            }

            if(Temp_Y > Target_G_Robot->Y)
            {
                Temp_Y_Accel = -DRONE_ACCEL;
            }
            else
            {
                Temp_Y_Accel = DRONE_ACCEL;
            }
        }

        float Movement_Dist = sqrt(Temp_X_Accel * Temp_X_Accel + Temp_Y_Accel * Temp_Y_Accel);
        if(Movement_Dist > DRONE_ACCEL)
        {
            Temp_X_Accel = Temp_X_Accel / (Movement_Dist / DRONE_ACCEL);
            Temp_Y_Accel = Temp_Y_Accel / (Movement_Dist / DRONE_ACCEL);
        }

        Current_Drone->X_Speed = Current_Drone->X_Speed + Temp_X_Accel;
        Current_Drone->Y_Speed = Current_Drone->Y_Speed + Temp_Y_Accel;

        Current_Drone->X = Current_Drone->X + Current_Drone->X_Speed;
        Current_Drone->Y = Current_Drone->Y + Current_Drone->Y_Speed;

        float A = Current_Drone->X - Target_G_Robot->X;
        float B = Current_Drone->Y - Target_G_Robot->Y;
        if(sqrt(A * A + B * B) / METER_PIXEL_DIST < TOP_TOUCH_MAX_OFFSET)
        {
            // Determine if drone is moving slow enough compared to ground robot to turn it
            float Drone_Movement_X = Current_Drone->X_Speed;
            float Drone_Movement_Y = Current_Drone->Y_Speed;
            float G_Robot_Movement_X = sin(Target_G_Robot->Rotation) * GROUND_ROBOT_PIXEL_SPEED;
            float G_Robot_Movement_Y = cos(Target_G_Robot->Rotation) * GROUND_ROBOT_PIXEL_SPEED;
            float A = Drone_Movement_X - G_Robot_Movement_X;
            float B = Drone_Movement_Y - G_Robot_Movement_Y;

            if(sqrt(A * A + B * B) / METER_PIXEL_DIST < TOP_TOUCH_MAX_SPEED_DIFF)
            {
                float Rotation = static_cast<int>(Target_G_Robot->Rotation) % 360;
                if(Rotation < 360)
                    Rotation += 360;

                if((Rotation < 180 - 45 || Rotation > 180 + 45))
                {
                    Target_G_Robot->Mode = TOP_TOUCH_MODE;
                    Target_G_Robot->Frames_To_Turn = (TOP_TOUCH_DEGREES / GROUND_ROBOT_TURN_SPEED) * COMP_FRAME_RATE;
                }
            }
        }


    }

    // Move the ground robots
    for(int index = 0; index < GROUND_ROBOT_NUMBER; index++)
    {
        Ground_Robot_Data * Current_G_Robot;
        float Prev_X = 0;
        float Prev_Y = 0;


        Current_G_Robot = &Array_Of_Frames[Current_Frame].Ground_Robots[index];
        if(Current_G_Robot->Still_In == true)
        {
            Prev_X = Current_G_Robot->X;
            Prev_Y = Current_G_Robot->Y;


            if(Current_G_Robot->Mode == FORWARD_MODE)
            {
                // If its time to reverse
                if(Current_G_Robot->Last_Frame_Reversed < Current_Frame - (COMP_FRAME_RATE * REVERSE_INTERVAL))
                {
                    Current_G_Robot->Mode = REVERSE_MODE;
                    Current_G_Robot->Last_Frame_Reversed = Current_Frame;
                    Current_G_Robot->Frames_To_Turn = (REVERSE_DEGREES / GROUND_ROBOT_TURN_SPEED) * COMP_FRAME_RATE;
                }
                else if(Current_G_Robot->Last_Frame_Noise < Current_Frame - (COMP_FRAME_RATE * NOISE_INTERVAL))
                {
                    Current_G_Robot->Mode =  NOISE_MODE;
                    Current_G_Robot->Last_Frame_Noise = Current_Frame;
                    Current_G_Robot->Frames_To_Turn = ((rand() % (static_cast<int>(NOISE_MAX_ROTATION - NOISE_MIN_ROTATION) + 1)) / GROUND_ROBOT_TURN_SPEED) * COMP_FRAME_RATE;
                }

                // Assign new X and Y to ground robot
                Current_G_Robot->X = Current_G_Robot->X + sin(Current_G_Robot->Rotation * TO_RADIANS) * GROUND_ROBOT_PIXEL_SPEED;
                Current_G_Robot->Y = Current_G_Robot->Y + cos(Current_G_Robot->Rotation * TO_RADIANS) * GROUND_ROBOT_PIXEL_SPEED;

                for(int index2 = 0; index2 < OBSTACLE_ROBOT_NUMBER; index2++)
                {
                    float A;
                    float B;
                    Obstacle_Robot_Data * Current_O_Robot;

                    Current_O_Robot = &Array_Of_Frames[Current_Frame].Obstacle_Robots[index2];
                    A = Current_G_Robot->X - Current_O_Robot->X;
                    B = Current_G_Robot->Y - Current_O_Robot->Y;
                    if(sqrt(A * A + B * B) < (GROUND_ROBOT_DIA / 2 + OBSTACLE_ROBOT_DIA / 2) * METER_PIXEL_DIST)
                    {
                        Current_G_Robot->X = Prev_X;
                        Current_G_Robot->Y = Prev_Y;
                        Current_G_Robot->Frames_To_Turn = (COLLISION_DEGREES / GROUND_ROBOT_TURN_SPEED) * COMP_FRAME_RATE;
                        Current_G_Robot->Mode = COLLISION_MODE;
                    }
                }

                for(int index2 = 0; index2 < GROUND_ROBOT_NUMBER; index2++)
                {
                    float A;
                    float B;
                    Ground_Robot_Data * Current_G_Robot2;
                    Current_G_Robot2 = &Array_Of_Frames[Current_Frame].Ground_Robots[index2];
                    if(Current_G_Robot2->Still_In == true)
                    {
                        if(index2 != index)
                        {
                            A = Current_G_Robot->X - Current_G_Robot2->X;
                            B = Current_G_Robot->Y - Current_G_Robot2->Y;
                            if(sqrt(A * A + B * B) < (GROUND_ROBOT_DIA / 2 + OBSTACLE_ROBOT_DIA / 2) * METER_PIXEL_DIST)
                            {
                                Current_G_Robot->X = Prev_X;
                                Current_G_Robot->Y = Prev_Y;
                                Current_G_Robot->Frames_To_Turn = (COLLISION_DEGREES / GROUND_ROBOT_TURN_SPEED) * COMP_FRAME_RATE;
                                Current_G_Robot->Mode = COLLISION_MODE;
                            }
                        }
                    }
                }
            }

        else if(Current_G_Robot->Mode == REVERSE_MODE)
        {
            // Turn
            Current_G_Robot->Rotation = Current_G_Robot->Rotation - (GROUND_ROBOT_TURN_SPEED / COMP_FRAME_RATE);

            // Decrement the frames left in the turn
            Current_G_Robot->Frames_To_Turn = Current_G_Robot->Frames_To_Turn - 1;

            // If the turn is done
            if(Current_G_Robot->Frames_To_Turn <= 0)
            {
                // Start driving forward again
                Current_G_Robot->Mode = FORWARD_MODE;
            }
        }
        else if(Current_G_Robot->Mode == NOISE_MODE)
        {
            // Turn
            Current_G_Robot->Rotation = Current_G_Robot->Rotation - (GROUND_ROBOT_TURN_SPEED / COMP_FRAME_RATE);

            // Decrement the frames left in the turn
            Current_G_Robot->Frames_To_Turn = Current_G_Robot->Frames_To_Turn - 1;

            // If the turn is done
            if(Current_G_Robot->Frames_To_Turn <= 0)
            {
                // Start driving forward again
                Current_G_Robot->Mode = FORWARD_MODE;
            }
        }
        else if(Current_G_Robot->Mode == COLLISION_MODE)
        {
            // Turn
            Current_G_Robot->Rotation = Current_G_Robot->Rotation - (GROUND_ROBOT_TURN_SPEED / COMP_FRAME_RATE);

            // Decrement the frames left in the turn
            Current_G_Robot->Frames_To_Turn = Current_G_Robot->Frames_To_Turn - 1;

            // If the turn is done
            if(Current_G_Robot->Frames_To_Turn <= 0)
            {
                // Start driving forward again
                Current_G_Robot->Mode = FORWARD_MODE;
            }
        }
        else if(Current_G_Robot->Mode == TOP_TOUCH_MODE)
        {
            // Turn
            Current_G_Robot->Rotation = Current_G_Robot->Rotation - (GROUND_ROBOT_TURN_SPEED / COMP_FRAME_RATE);

            // Decrement the frames left in the turn
            Current_G_Robot->Frames_To_Turn = Current_G_Robot->Frames_To_Turn - 1;

            // If the turn is done
            if(Current_G_Robot->Frames_To_Turn <= 0)
            {
                // Start driving forward again
                Current_G_Robot->Mode = FORWARD_MODE;
            }
        }
    }
    }
    // Move the obstacle robots
    for(int index = 0; index < OBSTACLE_ROBOT_NUMBER; index++)
    {
        Obstacle_Robot_Data * Current_O_Robot;
        float Prev_X = 0;
        float Prev_Y = 0;
        int Prev_Rotation = 0;

        Current_O_Robot = &Array_Of_Frames[Current_Frame].Obstacle_Robots[index];

        Prev_X = Current_O_Robot->X;
        Prev_Y = Current_O_Robot->Y;
        Prev_Rotation = Current_O_Robot->Rotation;

        float Rotation_Change = DEGREES_IN_CIRCLE / COMP_FRAME_RATE / OBSTACLE_ROBOT_FULL_CIRLE_TIME;
        Current_O_Robot->Rotation = Current_O_Robot->Rotation - Rotation_Change;
        Current_O_Robot->X = Current_O_Robot->X + sin(Current_O_Robot->Rotation * TO_RADIANS) * OBSTACLE_ROBOT_PIXEL_SPEED;
        Current_O_Robot->Y = Current_O_Robot->Y + cos(Current_O_Robot->Rotation * TO_RADIANS) * OBSTACLE_ROBOT_PIXEL_SPEED;

        for(int index2 = 0; index2 < GROUND_ROBOT_NUMBER; index2++)
        {
            float A;
            float B;
            Ground_Robot_Data * Current_G_Robot;

            Current_G_Robot = &Array_Of_Frames[Current_Frame].Ground_Robots[index2];
            if(Current_G_Robot->Still_In == true)
            {
                A = Current_O_Robot->X - Current_G_Robot->X;
                B = Current_O_Robot->Y - Current_G_Robot->Y;
                if(sqrt(A * A + B * B) < (GROUND_ROBOT_DIA / 2 + OBSTACLE_ROBOT_DIA / 2) * METER_PIXEL_DIST)
                {
                    Current_O_Robot->X = Prev_X;
                    Current_O_Robot->Y = Prev_Y;
                    Current_O_Robot->Rotation = Prev_Rotation;
                }
            }
        }
    }

    for(int index = 0; index < GROUND_ROBOT_NUMBER; index++)
    {
        Ground_Robot_Data * Current_G_Robot;
        Current_G_Robot = &Array_Of_Frames[Current_Frame].Ground_Robots[index];
        if(Current_G_Robot-> X < FIELD_X || Current_G_Robot->X > FIELD_X + FIELD_WIDTH || Current_G_Robot->Y > FIELD_Y + FIELD_HEIGHT)
        {
            Current_G_Robot->Still_In = false;
        }
        else if(Current_G_Robot->Y < FIELD_Y)
        {
            Current_G_Robot->Still_In = false;
            Current_G_Robot->Scored = true;
        }
    }

    bool done = true;
    for(int index = 0; index < GROUND_ROBOT_NUMBER; index++)
    {
        Ground_Robot_Data * Current_G_Robot;
        Current_G_Robot = &Array_Of_Frames[Current_Frame].Ground_Robots[index];
        if(Current_G_Robot->Still_In == true)
        {
            done = false;
        }
    }
    if(done == true && Has_Ended == false)
    {
        Frame_Ended = Current_Frame;
        Has_Ended = true;
    }
}



void Init()
{
    for(int index = 0; index < FRAME_LIMIT + 1; index++)
    {
        Array_Of_Frames[index].Ground_Robots.resize(GROUND_ROBOT_NUMBER);
        Array_Of_Frames[index].Obstacle_Robots.resize(OBSTACLE_ROBOT_NUMBER);
        Array_Of_Frames[index].Drones.resize(DRONE_NUMBER);
    }

    for(int index = 0; index < GROUND_ROBOT_NUMBER; index++)
    {
        float Temp_Rotation = index * DEGREES_IN_CIRCLE / GROUND_ROBOT_NUMBER;
        Array_Of_Frames[0].Ground_Robots[index].X = ARENA_WIDTH / 2 + sin(Temp_Rotation * TO_RADIANS) *  METER_PIXEL_DIST;
        Array_Of_Frames[0].Ground_Robots[index].Y = ARENA_HEIGHT / 2 + cos(Temp_Rotation * TO_RADIANS) *  METER_PIXEL_DIST;
        Array_Of_Frames[0].Ground_Robots[index].Rotation = Temp_Rotation;
        Array_Of_Frames[0].Ground_Robots[index].Color = (index % 2 == 0) ? RED_ID : BLUE_ID;
        Array_Of_Frames[0].Ground_Robots[index].Frames_To_Turn = 0;
        Array_Of_Frames[0].Ground_Robots[index].Turn_Direction_Mult = 0;
        Array_Of_Frames[0].Ground_Robots[index].Still_In = true;
        Array_Of_Frames[0].Ground_Robots[index].Scored = false;
        Array_Of_Frames[0].Ground_Robots[index].Mode = FORWARD_MODE;
        Array_Of_Frames[0].Ground_Robots[index].Last_Frame_Reversed = 0;
        Array_Of_Frames[0].Ground_Robots[index].Last_Frame_Noise = 0;
        Array_Of_Frames[0].Ground_Robots[index].Time_Left_To_Rotate = 0;
    }


    for(int index = 0; index < OBSTACLE_ROBOT_NUMBER; index++)
    {
        float Temp_Rotation = index * DEGREES_IN_CIRCLE / OBSTACLE_ROBOT_NUMBER;
        float Obstacle_Robot_Direction = Temp_Rotation - 90;
        Array_Of_Frames[0].Obstacle_Robots[index].X = FIELD_X + FIELD_WIDTH / 2 + sin(Temp_Rotation * TO_RADIANS) * METER_PIXEL_DIST * OBSTACLE_ROBOT_PATH_RADIUS;
        Array_Of_Frames[0].Obstacle_Robots[index].Y = FIELD_Y + FIELD_HEIGHT / 2 + cos(Temp_Rotation * TO_RADIANS) * METER_PIXEL_DIST * OBSTACLE_ROBOT_PATH_RADIUS;
        Array_Of_Frames[0].Obstacle_Robots[index].Rotation = Obstacle_Robot_Direction;
    }


    for(int index = 0; index < DRONE_NUMBER; index++)
    {
        float Temp_Rotation = index * DEGREES_IN_CIRCLE / GROUND_ROBOT_NUMBER;
        Array_Of_Frames[0].Drones[index].X = FIELD_X + FIELD_WIDTH + (DRONE_DIA * METER_PIXEL_DIST);
        Array_Of_Frames[0].Drones[index].Y = FIELD_Y + FIELD_HEIGHT / 2;
        Array_Of_Frames[0].Drones[index].X_Speed = 0;
        Array_Of_Frames[0].Drones[index].Y_Speed = 0;
        Array_Of_Frames[0].Drones[index].Target_Robot = 0;
        Array_Of_Frames[0].Drones[index].Ok_To_Leave = true;
    }
    Frame_Ended = FRAME_LIMIT;
    Has_Ended = false;
    return;
}



void Reload_Match()
{
}



void Init_Mults()
{
    for(int counter = 0; counter < DATA_SET_COUNT; counter++)
    {
        Gen_Random_Set(counter);
        Data_Sets[counter].Results.resize(MATCHES_PER_SET);
    }
}



void Score_Match()
{
    int Ground_Robots_Scored = 0;

    // Count ground robots scored
    for(int counter = 0; counter < GROUND_ROBOT_NUMBER; counter++)
    {
        if(Array_Of_Frames[Current_Frame].Ground_Robots[counter].Scored == true)
        {
            Ground_Robots_Scored++;
        }
    }
    Data_Sets[Current_Data_Set].Results[Current_Match_In_Set].Ground_Robots_Scored = Ground_Robots_Scored;
    Data_Sets[Current_Data_Set].Results[Current_Match_In_Set].Time_Taken = Frame_Ended;
    // Start the score with a starting score
    float Temp_Score = MATCH_START_SCORE;

    // Add points per robot scored
    Temp_Score = Temp_Score + (Ground_Robots_Scored * ROBOT_SCORED_POINTS);

    // Subtract points per robot not scored
    Temp_Score = Temp_Score + (GROUND_ROBOT_NUMBER - Ground_Robots_Scored) * ROBOT_NOT_SCORED_POINTS;

    // Subtract points per minute used in the match
    Temp_Score = Temp_Score + static_cast<int>(Frame_Ended / COMP_FRAME_RATE / SECONDS_IN_MINUTE) * MINUTE_USED_POINTS;

    // Set the result's score to Temp_Score
    Data_Sets[Current_Data_Set].Results[Current_Match_In_Set].Score = Temp_Score;
}



void Calc_Set_Ave()
{
    float Robots_Scored_Sum = 0;
    float Time_Used_Sum = 0;
    float Score_Sum = 0;
    for(int counter = 0; counter < MATCHES_PER_SET; counter++)
    {
        Robots_Scored_Sum = Robots_Scored_Sum + Data_Sets[Current_Data_Set].Results[counter].Ground_Robots_Scored;
        Time_Used_Sum = Time_Used_Sum + Data_Sets[Current_Data_Set].Results[counter].Time_Taken;
        Score_Sum = Score_Sum + Data_Sets[Current_Data_Set].Results[counter].Score;
    }
    Data_Sets[Current_Data_Set].Ave_Robots = Robots_Scored_Sum / MATCHES_PER_SET;
    Data_Sets[Current_Data_Set].Ave_Time_Taken = Time_Used_Sum / MATCHES_PER_SET;
    Data_Sets[Current_Data_Set].Ave_Score = Score_Sum / MATCHES_PER_SET;
}



void Output_Set(int index)
{
    cout << "Set " << index << endl;
    cout << "Green Line Mult: " << Data_Sets[index].Green_Line_Mult << endl;
    cout << "Dist to closest edge Mult: " << Data_Sets[index].Dist_From_Edge_Mult << endl;
    cout << "Dist from drone mult: " << Data_Sets[index].Dist_From_Drone_Mult << endl;
    cout << "Robots scored ave: " << Data_Sets[index].Ave_Robots << endl;
    cout << "Time used ave: " << Data_Sets[index].Ave_Time_Taken / COMP_FRAME_RATE / SECONDS_IN_MINUTE << endl;
    cout << "Score ave: " << Data_Sets[index].Ave_Score << endl;
    cout << endl << endl;
}



void Sort_Sets_By_Score()
{
    Data_Set Temp_Set;
    for(int counter1 = 0; counter1 < DATA_SET_COUNT; counter1++)
    {
        for(int counter2 = 0; counter2 < DATA_SET_COUNT - counter1; counter2++)
        {
            if(Data_Sets[counter2].Ave_Score < Data_Sets[counter2 + 1].Ave_Score)
            {
                Temp_Set = Data_Sets[counter2];
                Data_Sets[counter2] = Data_Sets[counter2 + 1];
                Data_Sets[counter2 + 1] = Temp_Set;
            }
        }
    }
/*
    for(int counter1 = 0; counter1 < DATA_SET_COUNT; counter1++)
    {
        Output_Set(counter1);
    }
    */
    //Sleep(500000);

}



void Make_New_Sets()
{
    for(int counter = 0; counter < DATA_SET_COUNT / 2; counter++)
    {
        Data_Sets[counter + DATA_SET_COUNT / 2].Dist_From_Drone_Mult = (Data_Sets[counter].Dist_From_Drone_Mult + Data_Sets[counter + 1].Dist_From_Drone_Mult) / 2;
        Data_Sets[counter + DATA_SET_COUNT / 2].Dist_From_Edge_Mult = (Data_Sets[counter].Dist_From_Edge_Mult + Data_Sets[counter + 1].Dist_From_Edge_Mult) / 2;
        Data_Sets[counter + DATA_SET_COUNT / 2].Green_Line_Mult = (Data_Sets[counter].Green_Line_Mult + Data_Sets[counter + 1].Green_Line_Mult) / 2;
    }
    for(int counter = round(static_cast<float>(DATA_SET_COUNT) / 4 * 3); counter < DATA_SET_COUNT; counter++)
    {
        Gen_Random_Set(counter);
    }
}



void Gen_Random_Set(int index)
{
    float Green_Line_Range = ((GREEN_LINE_MULT_MAX - GREEN_LINE_MULT_MIN) + 1);
    Data_Sets[index].Green_Line_Mult = static_cast<float>(rand()) / RAND_MAX * Green_Line_Range + GREEN_LINE_MULT_MIN;

    float Dist_To_Closest_Edge_Range = ((DIST_FROM_EDGE_MAX - DIST_FROM_EDGE_MIN) + 1);
    Data_Sets[index].Dist_From_Edge_Mult = static_cast<float>(rand()) / RAND_MAX * Dist_To_Closest_Edge_Range + DIST_FROM_EDGE_MIN;

    float Dist_From_Drone_Range = ((DIST_FROM_DRONE_MAX - DIST_FROM_DRONE_MIN) + 1);
    Data_Sets[index].Dist_From_Drone_Mult = static_cast<float>(rand()) / RAND_MAX * Dist_From_Drone_Range + DIST_FROM_DRONE_MIN;
}



HBITMAP CreateDIBSectionFunction(HDC hdc, int W,  int H)
{
	HBITMAP hbm;
	BITMAPINFO bmi;
  			BITMAPINFOHEADER bmih = {0};
			bmih.biSize = sizeof(BITMAPINFOHEADER);
			bmih.biWidth = W;
			bmih.biHeight = -H;
			bmih.biPlanes = 1;
			bmih.biBitCount = 32;
			bmih.biCompression = BI_RGB;
			bmih.biXPelsPerMeter = 0;
			bmih.biYPelsPerMeter = 0;
			bmih.biClrUsed = 0;
			bmih.biClrImportant = 0;
			bmih.biSizeImage = W * H * 4;
			bmi.bmiHeader = bmih;
  			hbm = CreateDIBSection(((hdc==NULL) ? GetDC(NULL) : hdc), &bmi, DIB_RGB_COLORS, NULL, NULL, 0);
  			return hbm;
}
