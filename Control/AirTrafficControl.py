############################################
# This file contains a wrapper class
# for DroneKit related operations
# for our drone.
############################################
# Multi-Rotor Robot Design Team
# Missouri University of Science Technology
# Spring 2017
# Lucas Coon, Mark Raymond Jr.
# pylint: disable=C, F, I, R, W

from datetime import datetime, timedelta
from os import system
from time import sleep

import dronekit
import math
import os
import time

global DESIRED_ATTITUDE = StandardAttitudes.level
global DESIRED_THRUST = StandardThrusts.none
global LAST_ATTITUDE = StandardAttitudes.level
global LAST_THRUST = StandardThrusts.none

class DroneAttitude():

  def __init__(self, pitch, yaw, roll):
    self.x = pitch
    self.y = yaw
    self.z = roll
    self.pitch = math.radians(pitch)
    self.yaw = math.radians(yaw)
    self.roll = math.radians(roll)
    self.quaternion = self.get_quaternion()

  def get_quaternion(self):
    q = []

    t0 = math.cos(self.yaw * 0.5)
    t1 = math.sin(self.yaw * 0.5)
    t2 = math.cos(self.roll * 0.5)
    t3 = math.sin(self.roll * 0.5)
    t4 = math.cos(self.pitch * 0.5)
    t5 = math.sin(self.pitch * 0.5)

    w = t0 * t2 * t4 + t1 * t3 * t5
    x = t0 * t3 * t4 - t1 * t2 * t5
    y = t0 * t2 * t5 + t1 * t3 * t4
    z = t1 * t2 * t4 - t0 * t3 * t5

    q.append(w)
    q.append(x)
    q.append(y)
    q.append(z)

    return q


class StandardAttitudes(object):
  level = DroneAttitude(0,0,0)
  forward = DroneAttitude(-5,0,0) # -5 degrees.
  backward = DroneAttitude(5,0,0)  # +5 degrees.
  turn = DroneAttitude(-3, 0, 0)
  bob_right = DroneAttitude(0, 0, 4.25)
  bob_left = DroneAttitude(0, 0, -3)

  
class StandardThrusts(object):
  none = 0.00
  hover = 0.50
  takeoff = 1.00
  low_speed = 0.55
  med_speed = 0.65
  high_speed = 0.75

class VehicleStates(object):
  hover = "HOVER"
  flying = "FLYING"
  unknown = "UNKNOWN"
  avoidance = "AVOIDANCE"
  landed = "LANDED"

class Tower(object):
  SERIAL_PORT = "/dev/ttyS1"
  BAUD_RATE = 57600
  SIMULATOR = "127.0.0.1:14551"
  STANDARD_ATTITUDE_BIT_FLAGS = 0b00111111
  FLIP_ATTITUDE_BIT_FLAGS = 0b00111000
  STANDARD_THRUST_CHANGE = 0.05
  MAX_TURN_TIME = 5
  MAX_CLIMB_RATE = 0.50
  LAND_ALTITUDE = 0.5
  TURN_START_VELOCITY = 3
  TURN_RADIUS = 0.5 # Meters
  ANGLE_INCREMENT = 1.1
  ANGLE_DECREMENT = 0.9

  def __init__(self):
    self.start_time = 0
    self.last_thrust = 0
    self.last_attitude = None
    self.vehicle_initialized = False

    self.vision_running = False
    self.avoidance_running = False
    self.stategy_running = False

    self.vehicle = None
    self.connected = False

    self.vehicle_busy = False
    self.vehicle_state = VehicleStates.unknown

  def enable_uart(self):
      print("\nEnabling serial UART connection on " + self.SERIAL_PORT + " at " + str(self.BAUD_RATE) + " baud...")
      return_code = system("stty -F " + self.SERIAL_PORT + " " + str(self.BAUD_RATE) + " raw -echo -echoe -echok -crtscts")
      if(return_code == 0):
        print("\nSucessfully enabled UART connection.")

  def initialize_drone(self):
    if(not self.vehicle_initialized):
      print("\nConnecting via " + self.SERIAL_PORT + " to flight controller...")
      self.vehicle = dronekit.connect(self.SERIAL_PORT, baud=self.BAUD_RATE, wait_ready=True)
      # self.vehicle = dronekit.connect(self.SIMULATOR, wait_ready=True)
      self.vehicle.mode = dronekit.VehicleMode("STABILIZE")
      self.vehicle_state = VehicleStates.landed
      self.connected = True
      self.vehicle_initialized = True
      self.start_time = time.time()
      self.last_attitude = StandardAttitudes.level
      print("\nSuccessfully connected to vehicle.")

  def arm_drone(self):
    self.vehicle.armed = True
    while(not self.vehicle.armed):
      sleep(1)
    
  def disarm_drone(self):
    self.vehicle.armed = False
    while(self.vehicle.armed):
      sleep(1)

  def hover(self, duration=None):
   hoverstart = time.time()
   hovertime = -(hoverstart-get_uptime())
   initX = self.vehicle.velocity_x(0)
   initY = self.vehicle.velocity_y(1)
   initZ = self.vehicle.velocity_z(0)
   MAX_DISTANCE = .5    
   Duration = 2.5
   self.vehicle_state = "HOVER"
   if (self.velocity*hovertime >= MAX_DISTANCE & hovertime == Duration):
       self.velocity(initX-self.vehicle.velocity_x(),initY-self.vehicle.velocity_y(),initZ-self.vehicle.velocity_z())
       time.sleep(Duration)
        
        
   
  def get_uptime(self):
    uptime = time.time() - self.start_time
    return uptime

  def takeoff(self, target_altitude):

    self.arm_drone()

    initial_alt = self.vehicle.location.global_relative_frame.alt
    
    while((self.vehicle.location.global_relative_frame.alt - initial_alt) < target_altitude):
      DESIRED_ATTITUDE = StandardAttitudes.level
      DESIRED_THRUST = StandardThrusts.takeoff
      sleep(1)

    print('Reached target altitude:{0:.2f}m'.format(self.vehicle.location.global_relative_frame.alt))

    self.hover()

  def fly_for_time(self, duration, direction, target_velocity, thrust=None):

    if(not thrust):
      thrust = self.last_thrust

    duration = timedelta(seconds=duration)
    end_manuever = datetime.now() + duration
    self.set_angle_thrust(direction, thrust)
    while(end_manuever <= datetime.now()):
      if(self.vehicle.airspeed > target_velocity):
        self.set_angle_thrust(direction, self.last_thrust - self.STANDARD_THRUST_CHANGE)
      elif(self.vehicle.airspeed < target_velocity):
        self.set_angle_thrust(direction, self.last_thrust + self.STANDARD_THRUST_CHANGE)
      sleep(1)

  def land(self):
    self.vehicle.mode = dronekit.VehicleMode("LAND")
    while((self.vehicle.location.global_relative_frame.alt) >= self.LAND_ALTITUDE):
      sleep(1)
    else:
      self.vehicle_state = VehicleStates.landed

  def do_circle_turn(self, desired_angle, direction, duration):
    if(duration > self.MAX_TURN_TIME):
      return

    desired_angle = math.radians(desired_angle)
      
    max_angle = desired_angle
    altitude_to_hold = self.vehicle.location.global_relative_frame.alt

    duration = timedelta(seconds=duration)
    end_manuever = datetime.now() + duration
    
    self.fly_for_time(1, StandardAttitudes.forward, self.TURN_START_VELOCITY)
    
    while(end_manuever <= datetime.now()):
      change_in_time = end_manuever - datetime.now()
      current_altitude = self.vehicle.location.global_relative_frame.alt

      roll_angle = max_angle * (math.cos(self.vehicle.airspeed * change_in_time.seconds) / self.TURN_RADIUS)
      pitch_angle = max_angle * (math.sin(self.vehicle.airspeed * change_in_time.seconds) / self.TURN_RADIUS)

      roll_angle = math.degrees(roll_angle)
      pitch_angle = math.degrees(pitch_angle)
      self.last_attitude.yaw = math.degrees(self.last_attitude.yaw)

      updated_attitude = DroneAttitude(pitch_angle, self.last_attitude.yaw, roll_angle)

      message = self.vehicle.message_factory.set_attitude_target_encode(
        0,                                        # Timestamp in milliseconds since system boot (not used).
        0,                                        # System ID
        0,                                        # Component ID
        self.STANDARD_ATTITUDE_BIT_FLAGS,         # Bit flags. For more info, see http://mavlink.org/messages/common#SET_ATTITUDE_TARGET.
        updated_attitude.quaternion,              # Attitude quaternion.
        0,                                        # Body roll rate.
        0,                                        # Body pitch rate.
        0,                                        # Body yaw rate.
        StandardThrusts.low_speed                # Collective thrust, from 0-1.
      )

      self.vehicle.send_mavlink(message)
      self.vehicle.commands.upload()

      print("Sent message.")

      if(current_altitude > altitude_to_hold):
        max_angle = desired_angle * self.ANGLE_INCREMENT
      elif(current_altitude < altitude_to_hold):
        max_angle = desired_angle * self.ANGLE_DECREMENT
    
      sleep(1)
      
    self.fly_for_time(1, StandardAttitudes.forward, self.vehicle.airspeed)


