<?php

/**
 * @file
 * Contains \Drupal\taarikh\Plugin\field\widget\TaarikhWidget.
 */

namespace Drupal\taarikh\Plugin\Field\FieldWidget;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Field\WidgetBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\datetime\Plugin\Field\FieldWidget\DateTimeWidgetBase;

/**
 * @FieldWidget(
 *   id = "Select_list_Hijri",
 *   module = "taarikh",
 *   label = @Translation("Select list Hijri"),
 *   field_types = {
 *     "datetime"
 *   }
 * )
 */
class Select_list_Hijri_Widget extends DateTimeWidgetBase {
  
  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {
    $element = parent::formElement($items, $delta, $element, $form, $form_state);
//    echo '<pre>';    print_r($form); exit;
    return $element;
  }

  /**
   * Validate the color text field.
   */
  public function validate($element, FormStateInterface $form_state) {
    $value = $element['#value'];
    if (strlen($value) == 0) {
      $form_state->setValueForElement($element, '');
      return;
    }
    if (!preg_match('/^#([a-f0-9]{6})$/iD', strtolower($value))) {
      $form_state->setError($element, t("Color must be a 6-digit hexadecimal value, suitable for CSS."));
    }
  }

}
