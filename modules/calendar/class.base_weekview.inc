<?
  // class to generate a weekview
   //
   // typical usage:
   //
   // $dummy = new weekview(25,7,2001);     // show July 25th 2001
   // $dummy->showGlobal  = "wim";          // for user wim
   // $dummy->owner = $g_user["name"];     // remember who is currently logged in
   // $dummy->generate();                  // generate the matrix
   // $dummy->dumpMatrix();                // show it to the audience ....
   //
   //

   class weekView
   {
    var $day          = 0;
    var $month        = 0;
    var $year         = 0;
    var $appointments = array();
    var $weekdates    = array();
    var $title        = "";
    var $showGlobal;
    var $owner;
    var $viewerWidth  = "100%";
    var $title        = "";

  /**
   * constructor
   * @param $day   (if 0, then current day)
   * @param $month (if 0, then current month)
   * @param $year  (if 0, then current year)
   */
    function weekView($day=0, $month=0, $year=0)
    {
      $this->year   = ($year  == 0) ? date("Y") : $year;
      $this->month  = ($month == 0) ? date("m") : $month;
      $this->day    = ($day   == 0) ? date("d") : $day;
    }

   /**
   * wrapper around the gathering functions
   * fills several info arrays
   */
    function generate()
    {
     $viewdate = $this->year."-".$this->month."-".$this->day;
     if ($viewdate!="")
      {
        $viewtime = mktime(12,12,0,substr($viewdate,5,2),substr($viewdate,8,2),substr($viewdate,0,4));
      }
      else
      {
        $viewdate = date("Y-m-d");
        $viewtime = time();
      }
      $weekday  = strftime("%w",$viewtime);

      if ($weekday==0) $weekday=7;

      for ($i=0; $i<=8;$i++)
      {
        $this->weekdates[$i]['date']  = date("Y-m-d" ,$viewtime-(86400*($weekday-$i)));
        $this->weekdates[$i]['day']   = strtolower(date("D" ,$viewtime-(86400*($weekday-$i))));
        $this->weekdates[$i]['stamp'] = $viewtime-(86400*($weekday-$i));
      }

      $this->appointments = $this->getAppointments($this->showGlobal, $this->weekdates[1]['date'], $this->weekdates[7]['date']);
    }

  /**
   * get the items for the specified week
   * fills an array with the items
   */
    function getAppointments($user, $begindate, $enddate)
    {
      // crosscomparison to match all appointments in range.
      $filter = "startdate<='$enddate' AND enddate>='$begindate'";
      if ($user!="all")
      {
        $filter.= " AND owner='$user'";
      }

      $schedule = &getNode("calendar.schedule");
      $recs = $schedule->selectDb($filter, "", "", "", "", "admin");

      // now add them to a nice array
      for ($i=0;$i<count($recs);$i++)
      {
        $date = sprintf("%d-%02d-%02d",$recs[$i]["startdate"]["year"],$recs[$i]["startdate"]["month"],$recs[$i]["startdate"]["day"]);

        if ($recs[$i]["allday"] != 1)
        {
          $daypart = $this->daypart($recs[$i]["starttime"]);
          $appointments[$date][$daypart][] = $recs[$i];
        }
        else
        {
          $appointments[$date]["allday"][] = $recs[$i];
        }
      }

      return $appointments;
    }

  /**
   * daypart
   * determine whether a time is in the morning, in the afternoon of in the evening
   * @returns morning, afternoon or evening
   */
    function daypart($time)
    {
     $hour = (int)$time["hours"];
     if ($hour < 12)
     {
       return "morning";
     }
     else if ($hour >= 12 && $hour < 17)
     {
       return "afternoon";
     }
     else
     {
       return "evening";
     }
    }

  /**
   * weekNav
   * navigate through weeks using prev/next week link
   * @returns html code containing links for previous week and next week
   */
   function weekNav()
   {
    global $PHP_SELF;

    $prevTime = $this->weekdates[0]['date'];
    $prevUrl  = $PHP_SELF."?view=weekview&viewdate=$prevTime&user=".$this->showGlobal;

    $nextTime = $this->weekdates[8]['date'];
    $nextUrl  = $PHP_SELF."?view=weekview&viewdate=$nextTime&user=".$this->showGlobal;

    $date     = $this->year."-".$this->month."-".$this->day;
    $this->title = "<font size='-1'><b>Week ".strftime("%W", mktime(0,0,0,substr($date,5,2), substr($date,8,2), substr($date,0,4)))."</b></font>";

    return "<font size='-1'>".href($prevUrl, text('houradmin_previousweek'))."</font>"."&nbsp;"."<font size='-1'>".href($nextUrl, text('houradmin_nextweek'))."</font>";
   }

  /**
   * dumpMatrix
   * the actual displaying function
   */
    function dumpMatrix()
    {
     global $PHP_SELF, $g_layout, $g_user;

      $buff   = "";
      $buff .= $g_layout->data_top();

      $buff .= $g_layout->tr_top();
      $buff .= $g_layout->ret_td_datatitle();

      for ($i=1;$i<=7;$i++)
      {
        $text  = text($this->weekdates[$i]["day"])."<br>(".substr($this->weekdates[$i]["date"],8,2)."-".substr($this->weekdates[$i]["date"],5,2).")";
        $href  = $PHP_SELF."?view=dayview&viewdate=".$this->weekdates[$i]["date"]."&dayview=".$this->m_showGlobal;
        $buff .= $g_layout->ret_td_datatitle(href($href, $text), 'align="center"');
      }
      $buff .= $g_layout->tr_bottom();

      $dayparts = array("allday","morning","afternoon","evening");

      foreach ($dayparts as $daypart_name)
      {

        // show different color when daypartname = allday.
        $buff .= $g_layout->tr_top($daypart_name=="allday");
        $buff .= $g_layout->ret_td(text($daypart_name));

        for ($i=1;$i<=7;$i++)
        {
          $dayappointments = $this->appointments[$this->weekdates[$i]["date"]][$daypart_name];
          $str="";
          for ($j=0;$j<count($dayappointments);$j++)
          {
            if ($g_user["name"]==$dayappointments[$j]["owner"])
            {
              // This is your own record, you may edit it.
              $link = href($PHP_SELF."?atkaction=edit&atkselector=".rawurlencode("schedule.id=".$dayappointments[$j]["id"]), $dayappointments[$j]['title'], SESSION_NESTED);
            }
            else
            {
              // this is somebody elses appointment. you may only view it if it is a public item.
              if ($dayappointments[$j]["publicitem"]==1)
              {
                $link = href($PHP_SELF."?atkaction=view&atkselector=".rawurlencode("schedule.id=".$dayappointments[$j]["id"]), $dayappointments[$j]['title'], SESSION_NESTED);
              }
              else
              {
                // not a public item. Show private appointment
                $link = text("private_appointment");
              }
            }

            if ($daypart_name!="allday")
            {
              $str.= "[".$dayappointments[$j]["starttime"]["hours"].":".$dayappointments[$j]["starttime"]["minutes"];
              $str.= "-".$dayappointments[$j]["endtime"]["hours"].":".$dayappointments[$j]["endtime"]["minutes"];
              $str.="] ";
            }
            $str.= $link;
            if ($this->showGlobal=="all") $str.=" (".$dayappointments[$j]["owner"].")";

            if ($j!=count($dayappointments)-1) $str.="<br>";
          }
          $buff .= $g_layout->ret_td($str);
        }
        $buff .= $g_layout->tr_bottom();
      }

      $buff .= $g_layout->tr_top();
      $buff .= $g_layout->ret_td_datatitle();
      for ($i=1;$i<=7;$i++)
      {
        $text  = text("add");
        $href  = $PHP_SELF."?atkaction=add&viewdate=".$this->weekdates[$i]["date"];
        $buff .= $g_layout->ret_td_datatitle(href($href, $text, SESSION_NESTED), 'align="center"');
      }
      $buff .= $g_layout->tr_bottom();

      $buff .= $g_layout->data_bottom();
      $buff .= "<br><br>";

      return $buff;
    }

   }

?>