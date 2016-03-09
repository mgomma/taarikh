<?php

/**
 * @file
 * Contains \Drupal\field_example\Controller\FieldExampleController.
 */

namespace Drupal\taarikh\Controller;

use Drupal\Core\Controller\ControllerBase;

/**
 * Returns responses for dblog routes.
 */
class TaarikhController extends ControllerBase {

  /**
   * A simple page to explain to the developer what to do.
   */
  public function description() {
    return array(
      '#markup' => t(
        "The Field taarikh hijri."),
    );
  }

}
