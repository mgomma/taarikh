<?php
/**
 * @file
 * Widget declarations and handlers for Hijri Date widget for Date fields.
 */

/**
 * Implements hook_field_widget_info().
 */
function taarikh_field_widget_info() {
  $settings = array(
    'settings' => array(
      'input_format' => 'Y/m/d',
      'input_format_custom' => '',
      'increment' => 15,
      'text_parts' => array(),
      'year_range' => '-3:+3',
      'label_position' => 'above',
    ),
    'behaviors' => array(
      'multiple values' => FIELD_BEHAVIOR_DEFAULT,
      'default value' => FIELD_BEHAVIOR_NONE,
    ),
  );

  $info = array(
    'taarikh_select' => array(
      'label' => t('Select list (Hijri)'),
      'field types' => array('date', 'datetime'),
    ) + $settings,
    'taarikh_popup' => array(
      'label' => t('Pop-up calendar (Hijri)'),
      'field types' => array('date', 'datetime'),
    ) + $settings,
  );
  return $info;
}

/**
 * Implements hook_field_widget_form().
 */
function taarikh_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {
  // Call date_combo hook_field_widget_form implementation.
  $element = date_field_widget_form($form, $form_state, $field, $instance, $langcode, $items, $delta, $element);
  return $element;
}

/**
 * Alter the date_combo element after the Date module is finished with it.
 *
 * @param array $element
 *   The $element array.
 * @param array $form_state
 *   A keyed array containing the current state of the form.
 * @param array $context
 *   An associative array containing the following keys:
 *   - field: The $field array.
 *   - instance: The $instance array.
 *   - form: Nested array of form elements that comprise the form.
 */
function taarikh_date_combo_process_alter(array &$element, array &$form_state, array $context) {
  $instance = $context['instance'];

  if ($instance['widget']['type'] == 'taarikh_popup') {
    $element['value']['#type'] = 'taarikh_popup';
    $element['value']['#theme_wrappers'] = array('taarikh_popup');
    $element['value']['#element_validate'] = array('taarikh_popup_widget_validate');
  }

  if ($instance['widget']['type'] == 'taarikh_select') {
    $element['value']['#type'] = 'taarikh_select';
    $element['value']['#theme_wrappers'] = array('taarikh_select');
    $element['value']['#element_validate'] = array('taarikh_select_widget_validate');
  }
}

/**
 * Form element validation handler for options element.
 */
function taarikh_select_widget_validate($element, &$form_state) {
  if ($element['#required'] && $element['#value'] == '') {
    form_error($element, t('%name field is required.', array('%name' => $element['#instance']['label'])));
  }

  // Convert date to gregorian for storage.
  $input = drupal_array_get_nested_value($form_state['values'], $element['#parents']);
  $date = taarikh_select_input_date($element, $input);

  if (!isset($date['year']) || !isset($date['month']) || !isset($date['day'])) {
    if (!empty($input['date'])) {
      $message = t('The value input for field %field is invalid.', array('%field' => $element['#instance']['label']));
      form_error($element, $message);
    }
    if ($element['#required']) {
      $message = t('A valid date is required for %title.', array('%title' => $element['#instance']['label']));
      form_error($element, $message);
    }

    return;
  }

  list($month, $day, $year) = explode("/", taarikh_api_convert_to_gregorian($date['year'], $date['month'], $date['day']));
  $date = date(DATE_FORMAT_DATETIME, mktime(0, 0, 0, $month, $day, $year));
  form_set_value($element, $date, $form_state);

  // This is hopefully temporary as we should rarely work with form_state[input]
  // we use this as date_combo_validate reads values from here for processing.
  //
  // This operation is necessary as without this, date module does not
  // understand the value stored in $element['#value'] as it is hardcoded to
  // only three of it's own widgets. For other widgets, it ends up calling
  // date_select_input_date() and the array below is compatible with that
  // function.
  $value = array(
    'year' => $year,
    'month' => $month,
    'day' => $day,
    'hour' => 0,
    'minute' => 0,
  );
  drupal_array_set_nested_value($form_state['input'], $element['#parents'], $value, TRUE);
}

/**
 * Form element validation handler for options element.
 */
function taarikh_popup_widget_validate($element, &$form_state) {
  if ($element['#required'] && $element['#value']['date'] == '') {
    form_error($element, t('%name field is required.', array('%name' => $element['#instance']['label'])));
  }

  // Convert date to gregorian for storage.
  $input = drupal_array_get_nested_value($form_state['values'], $element['#parents']);
  $date = taarikh_input_date($element, $input);

  if (!isset($date['year']) || !isset($date['month']) || !isset($date['day'])) {
    if (!empty($input['date'])) {
      $message = t('The value input for field %field is invalid.', array('%field' => $element['#instance']['label']));
      form_set_error($element, $message);
    }
    if ($element['#required']) {
      $message = t('A valid date is required for %title.', array('%title' => $element['#instance']['label']));
      form_set_error($element, $message);
    }

    return;
  }

  list($month, $day, $year) = explode("/", taarikh_api_convert_to_gregorian($date['year'], $date['month'], $date['day']));
  $date = date(DATE_FORMAT_DATETIME, mktime(0, 0, 0, $month, $day, $year));
  form_set_value($element, $date, $form_state);

  // This is hopefully temporary as we should rarely work with form_state[input]
  // we use this as date_combo_validate reads values from here for processing.
  //
  // This operation is necessary as without this, date module does not
  // understand the value stored in $element['#value'] as it is hardcoded to
  // only three of it's own widgets. For other widgets, it ends up calling
  // date_select_input_date() and the array below is compatible with that
  // function.
  $value = array(
    'year' => $year,
    'month' => $month,
    'day' => $day,
    'hour' => 0,
    'minute' => 0,
  );
  drupal_array_set_nested_value($form_state['input'], $element['#parents'], $value, TRUE);
}

/**
 * Implements hook_field_widget_properties_alter().
 *
 * Alters the widget properties of a field instance before it gets displayed.
 * This is used here to flag new entities so we can later tell if they need
 * default values.
 */
function taarikh_field_widget_properties_alter(&$widget, $context) {
  if (in_array($widget['type'], array('taarikh_select', 'taarikh_popup'))) {
    $entity = $context['entity'];
    $info = entity_get_info($context['entity_type']);
    $id = $info['entity keys']['id'];
    $widget['is_new'] = empty($entity->$id);
  }
}

/**
 * Implements hook_field_widget_error().
 */
function taarikh_field_widget_error($element, $error, $form, &$form_state) {
  form_error($element, $error['message']);
}
