<?php

/**
 * Module Project
 * Class project_personemployee
 *
 * This class makes it possible to relate employees to a project in a certain role(s).
 *
 * $Id$
 * @author L. Willems  <lineke@ibuildings.nl>
 * @version $Revision$
 *
 */

userelation("atkmanytoonerelation");

class project_personemployee extends atkNode
{
  function project_personemployee()
  {
    $this->atkNode("project_personemployee");

    $this->add(new atkManyToOneRelation("projectid", "project.project", AF_OBLIGATORY|AF_PRIMARY|AF_HIDE));
    $this->add(new atkManyToOneRelation("personid", "employee.employee", AF_OBLIGATORY|AF_PRIMARY|AF_RELATION_AUTOLINK))->addDestinationFilter("person.status='active'");
    $this->add(new atkManyToOneRelation("role", "project.role", AF_PRIMARY|AF_RELATION_AUTOLINK));
    $this->addFilter("personid.role = 'employee'");

    // If you may edit the project, you may also add contactpersons to it.
    $this->setSecurityAlias("project.project");
    foreach(array("add", "save", "update", "delete") as $action)
      $this->m_securityMap[$action] = "edit";

    $this->setTable("project_person", "project_person");
  }

  function action_delete(&$handler)
  {
    // When deleting, the above filter won't work. But that doesn't matter, we
    // don't need it anyway.
    $this->m_flags |= NF_NO_FILTER;
    return $handler->action_delete();
  }

}

?>