<?php

/**
 * @file
 * Contains Drupal\taarikh\Plugin\Field\FieldFormatter\TaarikhFormatter.
 */

namespace Drupal\taarikh\Plugin\Field\FieldFormatter;
use Drupal\Core\Form\FormStateInterface;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Datetime\DrupalDateTime;
use \Drupal\datetime\Plugin\Field\FieldFormatter\DateTimeDefaultFormatter;


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
class TaarikhFormatter extends DateTimeDefaultFormatter {

  /**
   * {@inheritdoc}
   * Copied from parent class and edited to display the Hijri date.
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = array();

    foreach ($items as $delta => $item) {
      $output = '';

      if ($item->date) {
        /** @var \Drupal\Core\Datetime\DrupalDateTime $date */
        $date = $item->date;
        
        list($year,$month, $day) = explode("-", $date);
        $time = $date->format(' H:i:s');
        
        $hijri_date = taarikh_api_convert($year, $month, $day);
        $hijri_date_time = implode('-', $hijri_date).$time;
        $hijri_data_object = new DrupalDateTime($hijri_date_time);

        if ($this->getFieldSetting('datetime_type') == 'date') {
          // A date without time will pick up the current time, use the default.
          datetime_date_default_time($date);
        }
        $this->setTimeZone($date);

        $output = $this->formatDate($hijri_data_object);
      }

      // Display the date using theme datetime.
      $elements[$delta] = array(
        '#cache' => [
          'contexts' => [
            'timezone',
          ],
        ],
        '#theme' => 'time',
        '#text' => $output,
        '#html' => FALSE,
        '#attributes' => array(
          'datetime' => $hijri_data_object,
        ),
      );
      if (!empty($item->_attributes)) {
        $elements[$delta]['#attributes'] += $item->_attributes;
        // Unset field item attributes since they have been included in the
        // formatter output and should not be rendered in the field template.
        unset($item->_attributes);
      }
    }

    return $elements;
  }
}
