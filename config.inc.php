<?php
  //
  // ACHIEVO CONFIGURATION FILE 
  //
  // Change this file to fit your needs before using Achievo.
  // 
  

  // -------------------------------------------
  //           DATABASE CONFIGURATION
  // -------------------------------------------


  // The database to use. Default is MySQL. If you want to use PostgreSQL,
  // change this value to "pgsql". For Oracle, use "oci8".
  //
  $config_database = "mysql";

  // The database configuration. Specify the hostname of the database server,
  // the database to use and the user/password.
  //
  $config_databasehost = "localhost";
  $config_databasename = "achievo_tryllian";
  $config_databaseuser = "demo";
  $config_databasepassword = "demo";
  
  
  // -------------------------------------------
  //           LAYOUT CONFIGURATION
  // -------------------------------------------
  
  // In admin pages, Achievo shows you a number of records with previous and
  // next buttons. You can specify the number of records to show on a page.
  //
  $config_recordsperpage=25; 

  // The theme defines the layout of Achievo. You can see which 
  // themes there are in the directory themes. Users can choose their own 
  // theme in the user preferences section.
  //
  $config_defaulttheme = "rainyd";
  
  // The language of the application. You can use any language for which
  // a language file is present in the languages directory.
  //
  $config_languagefile="english.lng";


  // -------------------------------------------
  //           SECURITY CONFIGURATION
  // -------------------------------------------
  
  // The method to use for user/password validation.
  // Currently supported are:
  // - "db"  : the passwords are stored in the Achievo database.
  // - "pop3": the passwords are validated against a pop3 server.
  // Note that if you validate passwords using pop3, you still have to add
  // useraccounts in Achievo. If you don't, users can login, but won't 
  // have permission to do anything.
  //
  $config_authentication = "db";
  
  // If you set $config_authentication to "pop3", you must configure 
  // which server to use:
  //
  $config_auth_mail_server = "localhost";
  
  // The Achievo administrator password. 
  // Regardless of the authentication method chosen above, this password
  // will always allow you to login with userid 'administrator'. This is 
  // useful when you have an empty or corrupt database. 
  // Note that it has no effect when you use the 'user preferences' screen
  // to change the administrator password, since the administrator password
  // is fixed in this file.
  //
  // Note: As an extra security measure you could disable this once you have
  // set up the entire system and enable it only when you need it.
  // (disable it by putting // in front of it)
  //
  $config_administratorpassword = "demo";  
  
  
  // -------------------------------------------
  //            TIME REGISTRATION 
  // -------------------------------------------
  
  // The number of projects/phases to show in the 'recent projects/phases'
  // dropdown. Note: The more you specify here, the slower time registration 
  // gets (due to some javascript issues).
  //
  $config_numberofrecentprojects = 10;

  // The ammount of time that a user can book on a day before it is 
  // considered overtime. 
  //
  $config_overtimethreshold = 480;
  
  // Default view in time registration. Can be "week" or "day"
  //
  $config_timereg_defaultview = "day";
  
  // This variable indicates whether the user may directly register
  // time on each day in a week when he is in weekview. 
  // If set to false, the user must first go to the dayview of a day, 
  // before he may register time.
  //
  $config_timereg_week_bookable = true;
  
  $config_timereg_incompleteweeklock = false;
  
  // -------------------------------------------
  //           FINANCIAL CONFIGURATION
  // -------------------------------------------

  // Default currency and symbol to use.
  //
  $config_currency = "Euro";
  $config_currency_symbol = "EU";    
  
  // -------------------------------------------
  //            EXTERNAL MODULES
  // -------------------------------------------
  
  // Use the mnodule function if you want to use Achievo modules.
  // Specify the module name as first parameter, and the path where
  // it can be found as the second parameter.
  //
  module("tryllian","../achievo_modules/tryllian");
  
  // -------------------------------------------
  //            END OF CONFIGURATION
  // -------------------------------------------

  // Leave this line in place, it configures the backend of Achievo.
  // Also, you should not change the atkconf.inc file, since that would 
  // break Achievo.
  include "atkconf.inc";


?>
