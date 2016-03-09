<?php
/**
 * @file
 * Formatter hooks to implement a date field.
 */

/**
 * Implements hook_field_formatter_info().
 */
function taarikh_field_formatter_info() {
  $formatters = array(
    'date_hijri' => array(
      'label' => t('Hijri Date'),
      'field types' => array('date', 'datestamp', 'datetime'),
      'settings' => array(
        'format_type' => 'long',
        'multiple_number' => '',
        'multiple_from' => '',
        'multiple_to' => '',
        'fromto' => 'both',
        'show_remaining_days' => FALSE,
      ),
    ),
  );
  return $formatters;
}

/**
 * Implements hook_field_formatter_settings_form().
 */
function taarikh_field_formatter_settings_form($field, $instance, $view_mode, $form, &$form_state) {
  module_load_include('inc', 'date', 'date_admin');

  $form = date_default_formatter_settings_form($field, $instance, $view_mode, $form, $form_state);
  return $form;
}

/**
 * Implements hook_field_formatter_settings_summary().
 */
function taarikh_field_formatter_settings_summary($field, $instance, $view_mode) {
  module_load_include('inc', 'date', 'date_admin');
  $summary = date_default_formatter_settings_summary($field, $instance, $view_mode);

  return implode('<br />', $summary);
}

/**
 * Implements hook_field_formatter_view().
 *
 * Useful values:
 *
 *   $entity->date_id
 *     If set, this will show only an individual date on a field with
 *     multiple dates. The value should be a string that contains
 *     the following values, separated with periods:
 *     - module name of the module adding the item
 *     - node nid
 *     - field name
 *     - delta value of the field to be displayed
 *     - other information the module's custom theme might need
 *
 *     Used by the calendar module and available for other uses.
 *     example: 'date:217:field_date:3:test'
 *
 *   $entity->date_repeat_show
 *     If true, tells the theme to show all the computed values
 *     of a repeating date. If not true or not set, only the
 *     start date and the repeat rule will be displayed.
 */
function taarikh_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  // We can just call date's implementation of this hook as the
  // functionality we need falls through in switch case's default
  // block anyway.
  // We convert the date for display in the hook called by
  // date_formatter_process()
  return date_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display);
}

/**
 * Implements hook_date_formatter_dates_alter().
 *
 * Alter the dates array created by date_formatter_process().
 */
function taarikh_date_formatter_dates_alter(&$dates, $context) {
  $format = $context['format'];

  if ($context['display']['type'] == 'date_hijri') {
    $date1 = $dates['value']['local']['object']->toArray();
    $date2 = $dates['value2']['local']['object']->toArray();
    $taarikh1 = taarikh_api_convert($date1['year'], $date1['month'], $date1['day']);
    $taarikh2 = taarikh_api_convert($date2['year'], $date2['month'], $date2['day']);

    $dates['value']['formatted_time'] = '';
    $dates['value2']['formatted_time'] = '';
    $dates['value']['formatted'] = taarikh_api_format($format, $taarikh1);
    $dates['value2']['formatted'] = taarikh_api_format($format, $taarikh2);
  }
}
