// runwayprefs.hxx - A number of classes to configure runway
// assignments by the AI code
//
// Written by Durk Talsma, started January 2005.
//
// Copyright (C) 2004 Durk Talsma.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
// $Id: runwayprefs.hxx,v 1.3 2006-03-04 08:49:36 durk Exp $

#ifndef _RUNWAYPREFS_HXX_
#define _RUNWAYPREFS_HXX_

#include <simgear/xml/easyxml.hxx>

typedef vector<time_t> timeVec;
typedef vector<time_t>::const_iterator timeVecConstIterator;

typedef vector<string> stringVec;
typedef vector<string>::iterator stringVecIterator;
typedef vector<string>::const_iterator stringVecConstIterator;


/***************************************************************************/
class ScheduleTime {
private:
  timeVec   start;
  timeVec   end;
  stringVec scheduleNames;
  double tailWind;
  double crssWind;
public:
  ScheduleTime() : tailWind(0), crssWind(0) {};
  ScheduleTime(const ScheduleTime &other);
  ScheduleTime &operator= (const ScheduleTime &other);
  string getName(time_t dayStart);

  void clear();
  void addStartTime(time_t time)     { start.push_back(time);            };
  void addEndTime  (time_t time)     { end.  push_back(time);            };
  void addScheduleName(const string& sched) { scheduleNames.push_back(sched);   };
  void setTailWind(double wnd)  { tailWind = wnd;                        };
  void setCrossWind(double wnd) { tailWind = wnd;                        };

  double getTailWind()  { return tailWind;                               };
  double getCrossWind() { return crssWind;                               };
};

//typedef vector<ScheduleTime> ScheduleTimes;
/*****************************************************************************/

class RunwayList
{
private:
  string type;
  stringVec preferredRunways;
public:
  RunwayList() {};
  RunwayList(const RunwayList &other);
  RunwayList& operator= (const RunwayList &other);

  void set(const string&, const string&);
  void clear();

  string getType() { return type; };
  stringVec *getRwyList() { return &preferredRunways;    };
  string getRwyList(int j) { return preferredRunways[j]; };
};

typedef vector<RunwayList> RunwayListVec;
typedef vector<RunwayList>::iterator RunwayListVectorIterator;
typedef vector<RunwayList>::const_iterator RunwayListVecConstIterator;


/*****************************************************************************/

class RunwayGroup
{
private:
  string name;
  RunwayListVec rwyList;
  int active;
  //stringVec runwayNames;
  int choice[2];
  int nrActive;

public:
  RunwayGroup() {};
  RunwayGroup(const RunwayGroup &other);
  RunwayGroup &operator= (const RunwayGroup &other);

  void setName(const string& nm) { name = nm;                };
  void add(const RunwayList& list) { rwyList.push_back(list);};
  void setActive(const string& aptId, double windSpeed, double windHeading, double maxTail, double maxCross, stringVec *curr);

  int getNrActiveRunways() { return nrActive;};
  void getActive(int i, string& name, string& type);

  string getName() { return name; };
  void clear() { rwyList.clear(); }; 
  //void add(string, string);
};

typedef vector<RunwayGroup> PreferenceList;
typedef vector<RunwayGroup>::iterator PreferenceListIterator;
typedef vector<RunwayGroup>::const_iterator PreferenceListConstIterator;

/******************************************************************************/

class FGRunwayPreference  : public XMLVisitor {
private:
  string value;
  string scheduleName;

  ScheduleTime comTimes; // Commercial Traffic;
  ScheduleTime genTimes; // General Aviation;
  ScheduleTime milTimes; // Military Traffic;
  ScheduleTime currTimes; // Needed for parsing;

  RunwayList  rwyList;
  RunwayGroup rwyGroup;
  PreferenceList preferences;
  

  time_t processTime(const string&);
  bool initialized;

public:
  FGRunwayPreference();
  FGRunwayPreference(const FGRunwayPreference &other);
  
  FGRunwayPreference & operator= (const FGRunwayPreference &other);
  ScheduleTime *getSchedule(const char *trafficType);
  RunwayGroup *getGroup(const string& groupName);
  bool available() { return initialized; };

 // Some overloaded virtual XMLVisitor members
  virtual void startXML (); 
  virtual void endXML   ();
  virtual void startElement (const char * name, const XMLAttributes &atts);
  virtual void endElement (const char * name);
  virtual void data (const char * s, int len);
  virtual void pi (const char * target, const char * data);
  virtual void warning (const char * message, int line, int column);
  virtual void error (const char * message, int line, int column);
};

#endif
