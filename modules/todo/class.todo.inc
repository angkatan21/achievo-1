<?

require_once("achievotools.inc");
userelation("atkmanytoonerelation");
userelation("atkonetomanyrelation");
useattrib("atkupdatestampattribute");
useattrib("atkdateattribute");
useattrib("atklistattribute");
useattrib("atktextattribute");

class todo extends atkNode
{
  var $prioId   = array(1,2,3,4,5);
  var $prioName = array("Highest","High","Normal","Low","Lowest");

  function todo()
  {
    global $atkaction, $g_user;

    $this->atkNode("todo", NF_TRACK_CHANGES);

    $this->add(new atkAttribute("id"                   , AF_AUTOKEY));
    $this->add(new atkAttribute("owner"                , AF_READONLY|AF_HIDE));
    $this->add(new atkManyToOneRelation("projectid"    , "project.project",AF_OBLIGATORY|AF_SEARCHABLE));
    $this->add(new atkAttribute("title"                , AF_OBLIGATORY));
    $this->add(new atkManyToOneRelation("assigned_to"  , "employee.employee", AF_SEARCHABLE));    
    $this->add(new atkDateAttribute("entrydate"        , "F d Y", "d F Y", 0, 0, AF_READONLY_EDIT|AF_HIDE_ADD));
    $this->add(new atkDateAttribute("duedate"          , "F d Y","d F Y", 0, 0, AF_OBLIGATORY));
    $this->add(new atkUpdateStampAttribute("updated"   , AF_HIDE|AF_FORCE_LOAD)); // force_load is necessary, because we need the value when copying the todo to the history table.
    $this->add(new atkListAttribute("priority"         , $this->prioName, $this->prioId, AF_OBLIGATORY));
    $this->add(new atkTextAttribute("description"      , AF_OBLIGATORY|AF_HIDE_LIST));
    $this->add(new atkListAttribute("status"           , array("todo_new", "todo_rejected", "todo_in_progress","todo_on_hold","todo_completed"), array(1,2,3,4,5),AF_SEARCHABLE));    
    $this->add(new atkOneToManyRelation("history","todo.todo_history","todoid",AF_HIDE_LIST|AF_CASCADE_DELETE));

    $this->setTable("todo", "todo");         
    $this->setOrder("duedate, priority");

  }

  function initial_values()
  {
    global $g_user;
    
    $entrydate = array("year"=>date("Y"),
                    "month"=>date("m"),
                    "day"=>date("d"));
    $nextweek = getdate(time()+60*60*24*7);
    $duedate = array("year"=>$nextweek['year'],
                  "month"=>$nextweek['mon'],
                  "day"=>$nextweek['mday']);
    return array("owner"=>$g_user["name"],                 
                 "entrydate"=>$entrydate,
                 "duedate"=>$duedate,
                 "updated"=>$date,
                 "priority"=>3);
  }    

  // overide display of priority (set high to a red font)
  function priority_display($rec)
  {
   $dummy  = $rec["priority"];
   $result = "";

   switch ($dummy)
   {
    case 1:
     $dummy = "<font color='#FF0000'>".$this->prioName[$dummy-1]."</font>";
     break;

    case 2:
     $dummy = "<font color='#A00000'>".$this->prioName[$dummy-1]."</font>";
     break;

    default:
     $dummy = $this->prioName[$rec["priority"]-1];
     break;
   }
   return $dummy;
  }

  function descriptor_def()
  {
   return "[title]";
  }

  // support function to add the priority header to a mail message
  function prioHeader($level)
  {
   $cr = chr(13).chr(10);
   
   switch ($level)
   {
    case 1:
     $mslevel = "High";
     break;

    case 2:    
     $mslevel = "High";
     break;

    case 3:
     $mslevel = "Normal";
     break;

    case 4:
     $mslevel = "Low";
     break;

    case 5:
     $mslevel = "Low";
     break;

    default:
     $level   = 3;
     $mslevel = "Normal";
   }

   $header  = "X-Priority: ".$level.$cr;
   $header .= "X-MSMail-Priority: ".$mslevel.$cr;  
   return $header;
  }
  
  function postAdd($rec)
  {
   global $g_db;

   // obtain some project info
   $assignTo = $rec["assigned_to"]["userid"];
   $assigner = $rec["owner"]; 
   $pid      = $rec["projectid"]["id"];

   // get project name
   $q       = "SELECT name FROM project WHERE id=$pid";
   $r       = $g_db->getRows($q);
   $project = $r[0]["name"];

   // now get her/his email address
   $q  = "SELECT email, name, userid FROM employee WHERE userid IN ('$assignTo','".$assigner."')";
   $emails  = $g_db->getrows($q);   
   
   // now assemble the mail body
   $subj  = text("todoadded").": ".$rec["title"];
    
   $body = text("todo").": ".$rec["title"]."\n";     
   $body.= text("description").": \n".$rec["description"]."\n\n";
   $body.= text("priority").": ".$this->prioName[$rec["priority"]-1]."\n";
   $body.= text("status").": ".$this->m_attribList["status"]->display($rec)."\n";
   if ($project!="") $body.= text("project").": ".$project."\n";
   $body.= text("duedate").": ".$this->m_attribList["duedate"]->display($rec)."\n";
     
   $body.= "\n";
   
   for ($i=0;$i<count($emails);$i++)
   {
     if ($emails[$i]["userid"]==$assigner) 
       $body.= text("todo_ownedby").": ".$emails[$i]["name"]."\n"; 
     if ($emails[$i]["userid"]==$assignTo) 
       $body.= text("todo_assignedto").": ".$emails[$i]["name"]."\n";
     $to[] = $emails[$i]["email"];
   }           

   // send mail   
   $header = $this->prioHeader($rec["priority"]); 
   if (is_array($to) && count($to)!="")
   {
     usermail(implode(",",$to), $subj, $body, $header);  
   }
  }
  
  function todoChanged($new, $old)
  {        
    foreach (array_keys($this->m_attribList) as $attribname)
    {
      if ($attribname!="updated") // leave out the updated attrib, for it always
                                  // changes, but that doesn't mean a change in 
                                  // the record.
      {
        $p_attrib = &$this->m_attribList[$attribname];
        if (!$p_attrib->equal($new,$old))
        {
          atkdebug($attribname." changed");
          return true;
        }
      }
    }
    return false;
  }
  
  function postUpdate($rec)
  {
   global $g_db, $g_user;

   if ($this->todoChanged($rec,$rec["atkorgrec"]))
   {    
     $owner = $rec["owner"];
     $newassign = $rec["assigned_to"]["userid"];
     $oldassign = $rec["atkorgrec"]["assigned_to"]["userid"];
     
     // read all relevant emails.
     $query = "SELECT DISTINCT name, email, userid 
               FROM employee 
               WHERE userid IN('$owner','".$g_user["name"]."','$newassign','$oldassign')";
     $emails    = $g_db->getrows($query);
     $pid  = $rec["projectid"]["id"];
     // get project name
     $query = "SELECT name FROM project WHERE id=$pid";
     $res   = $g_db->getrows($query);
     $project = $res[0]["name"];     
     
     $body = text("todo").": ".$rec["title"]."\n";     
     $body.= text("description").": \n".$rec["description"]."\n\n";
     $body.= text("priority").": ".$this->prioName[$rec["priority"]-1]."\n";
     $body.= text("status").": ".$this->m_attribList["status"]->display($rec)."\n";
     if ($project!="") $body.= text("project").": ".$project."\n";
     $body.= text("duedate").": ".$this->m_attribList["duedate"]->display($rec)."\n";
     
     $body.= "\n";
     for ($i=0;$i<count($emails);$i++)
     {
       if ($emails[$i]["userid"]==$owner) 
         $body.= text("todo_ownedby").": ".$emails[$i]["name"]."\n";
       if ($emails[$i]["userid"]==$oldassign) 
         $body.= text("todo_previouslyassignedto").": ".$emails[$i]["name"]."\n";         
       if ($emails[$i]["userid"]==$newassign) 
         $body.= text("todo_assignedto").": ".$emails[$i]["name"]."\n";
       if ($emails[$i]["userid"]==$g_user["name"]) 
         $body.= text("todo_changedby").": ".$emails[$i]["name"]."\n";
       $to[] = $emails[$i]["email"];
     }           
 
     // send mail     
     $header = $this->prioHeader($rec["priority"]);
     usermail(implode(",",$to),text("todochanged"), $body, $header);    
     
     // also store the old todo in the history table.
     $historynode = getNode("todo.todo_history");
     $rec["atkorgrec"]["todoid"]["id"] = $rec["id"]; // the todoid in the historytable
                                               // points to the current todoid.
     $historynode->addDb($rec["atkorgrec"]);
   }
  }

}

?>