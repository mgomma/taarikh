<?php

/**
 * @file
 * Contains Drupal\taarikh\Plugin\Field\FieldFormatter\SimpleTextFormatter.
 */

namespace Drupal\taarikh\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FormatterBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Datetime\DrupalDateTime;  


/**
 * Plugin implementation of the 'field_example_simple_text' formatter.
 *
 * @FieldFormatter(
 *   id = "Hijri",
 *   module = "taarikh",
 *   label = @Translation("Hijri date"),
 *   field_types = {
 *     "datetime"
 *   }
 * )
 */
class TaarikhFormatter extends FormatterBase {

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = array();

    foreach ($items as $delta => $item) {
      $elements[$delta] = array(
        // We create a render array to produce the desired markup,
        // "<p style="color: #hexcolor">The color code ... #hexcolor</p>".
        // See theme_html_tag().
        '#type' => 'html_tag',
        '#tag' => 'p',
        '#value' => $this->hijri_date_display($item->value),
      );
    }

    return $elements;
  }

  /**
   * display hijri date wiht the default format.
   * @param type $date
   * @return type
   */
  public function hijri_date_display($date) {     
    list($year,$month, $day) = explode("-", $date);
    $hijri_data = taarikh_api_convert($year, $month, $day);

    $hijri_data_object = new DrupalDateTime(implode('-', $hijri_data));
    return $hijri_data_object->format(DrupalDateTime::FORMAT);
  }

}
