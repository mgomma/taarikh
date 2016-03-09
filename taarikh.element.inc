<?php
/**
 * @file
 * Element declarations for Hijri Date widget for Date fields.
 */

/**
 * Implements hook_element_info().
 *
 * Set the #type to date_popup and fill the element #default_value with
 * a date adjusted to the proper local timezone in datetime format
 * (YYYY-MM-DD HH:MM:SS).
 *
 * The element will create two textfields, one for the date and one for the
 * time. The date textfield will include a jQuery popup calendar date picker,
 * and the time textfield uses a jQuery timepicker.
 *
 * NOTE - Converting a date stored in the database from UTC to the local zone
 * and converting it back to UTC before storing it is not handled by this
 * element and must be done in pre-form and post-form processing!!
 *
 * #date_timezone
 *   The local timezone to be used to create this date.
 *
 * #date_format
 *   Unlike earlier versions of this popup, most formats will work.
 *
 * #date_increment
 *   Increment minutes and seconds by this amount, default is 1.
 *
 * #date_year_range
 *   The number of years to go back and forward in a year selector,
 *   default is -3:+3 (3 back and 3 forward).
 */
function taarikh_element_info() {
  $element_info = array(
    '#input' => TRUE,
    '#tree' => TRUE,
    '#date_timezone' => date_default_timezone(),
    '#date_flexible' => 0,
    '#date_format' => variable_get('date_format_short', 'Y/m/d'),
    '#date_increment' => 1,
    '#date_year_range' => '-3:+3',
    '#date_label_position' => 'above',
  );

  $type['taarikh_select'] = array(
    '#process' => array('taarikh_select_element_process'),
    '#value_callback' => 'taarikh_select_element_value_callback',
    '#theme_wrappers' => array('taarikh_select'),
  ) + $element_info;
  $type['taarikh_popup'] = array(
    '#process' => array('taarikh_popup_element_process'),
    '#value_callback' => 'taarikh_popup_element_value_callback',
    '#theme_wrappers' => array('taarikh_popup'),
  ) + $element_info;

  return $type;
}

/**
 * Element value callback for taarikh_popup element.
 */
function taarikh_select_element_value_callback($element, $input = FALSE, &$form_state = array()) {
  // $granularity = date_format_order($element['#date_format']);
  $date = NULL;
  $return = array('year' => '', 'month' => '', 'day' => '');

  // Normal input from submitting the form element.
  // Check is_array() to skip the string input values created by Views pagers.
  // Those string values, if present, should be interpreted as empty input.
  if ($input !== FALSE && is_array($input)) {
    $return = $input;
    $date = taarikh_select_input_date($element, $input);

    // Date with errors won't re-display.
    if (!empty($date)) {
      list($month, $day, $year) = explode("/", taarikh_api_convert_to_gregorian($date['year'], $date['month'], $date['day']));
      $return = array(
        'year' => $year,
        'month' => $month,
        'day' => $day,
      );
    }
    elseif (!empty($input)) {
      $return = $input;
    }
  }
  // No input? Try the default value.
  elseif (!empty($element['#default_value'])) {
    $date = getdate(strtotime($element['#default_value']));
    $return = array(
      'year' => $date['year'],
      'month' => $date['mon'],
      'day' => $date['mday'],
    );
  }

  return $return;
}

/**
 * Javascript popup element processing.
 *
 * Add popup attributes to $element.
 */
function taarikh_select_element_process($element, &$form_state, $form) {
  if (date_hidden_element($element)) {
    return $element;
  }

  // Convert the date to gregorian if needed.
  // We check this based on the value of $element['#date_type'] which defaults
  // to gregorian if not present.
  // To specify a default date in hijri, set $element['#date_type'] to hijri.
  $date = $element['#value'];
  $entered = array_values(array_filter($date));
  if (!empty($entered) && (!isset($element['#date_type']) || $element['#date_type'] != 'hijri')) {
    $date = taarikh_api_convert($date['year'], $date['month'], $date['day']);
    $date = array(
      'year' => $date[0],
      'month' => $date[1],
      'day' => $date[2],
    );
  }

  $element['#tree'] = TRUE;
  $element['#theme_wrappers'] = array('date_select');

  $element += (array) taarikh_parts_element($element, $date, $element['#date_format']);

  // Store a hidden value for all date parts not in the current display.
  $granularity = date_format_order($element['#date_format']);
  foreach (date_nongranularity($granularity) as $field) {
    if ($field != 'timezone') {
      $element[$field] = array(
        '#type' => 'value',
        '#value' => 0,
      );
    }
  }
  if (isset($element['#element_validate'])) {
    array_push($element['#element_validate'], 'taarikh_select_validate');
  }
  else {
    $element['#element_validate'] = array('taarikh_select_validate');
  }

  return $element;
}

/**
 * Element value callback for taarikh_popup element.
 */
function taarikh_popup_element_value_callback($element, $input = FALSE, &$form_state = array()) {
  // We are not supporting time parts yet, and hence commenting this out.
  // $granularity = date_format_order($element['#date_format']);
  // $has_time = date_has_time($granularity);
  $has_time = FALSE;
  $date = NULL;
  $return = $has_time ? array('date' => '', 'time' => '') : array('date' => '');

  // Normal input from submitting the form element.
  // Check is_array() to skip the string input values created by Views pagers.
  // Those string values, if present, should be interpreted as empty input.
  if ($input !== FALSE && is_array($input)) {
    $return = $input;
    $date = taarikh_input_date($element, $input);

    // Date with errors won't re-display.
    if (!empty($date)) {
      list($month, $day, $year) = explode("/", taarikh_api_convert_to_gregorian($date['year'], $date['month'], $date['day']));
      $return['date'] = date(DATE_FORMAT_DATETIME, mktime(0, 0, 0, $month, $day, $year));
    }
    elseif (!empty($input)) {
      $return = $input;
    }
  }
  // No input? Try the default value.
  elseif (!empty($element['#default_value'])) {
    // We can assume that the default value will be in a format
    // YYYY-MM-DD HH:MM:SS.
    $default_value = explode(' ', $element['#default_value']);
    $return['date'] = $default_value[0];
    $return['time'] = $default_value[1];
  }

  return $return;
}

/**
 * Javascript popup element processing.
 *
 * Add popup attributes to $element.
 */
function taarikh_popup_element_process($element, &$form_state, $form) {
  if (date_hidden_element($element)) {
    return $element;
  }

  taarikh_add();
  module_load_include('inc', 'date_api', 'date_api_elements');

  $element['#tree'] = TRUE;
  $element['#theme_wrappers'] = array('taarikh_popup');

  if (!empty($element['#ajax'])) {
    $element['#ajax'] += array(
      'trigger_as' => array('name' => $element['#name']),
      'event' => 'change',
    );
  }

  $element['date'] = taarikh_process_date_part($element);

  if (isset($element['#element_validate'])) {
    array_push($element['#element_validate'], 'taarikh_element_validate');
  }
  else {
    $element['#element_validate'] = array('taarikh_element_validate');
  }

  return $element;
}

/**
 * Process the date portion of the element.
 */
function taarikh_process_date_part(&$element) {
  $date_granularity = date_popup_date_granularity($element);
  if (empty($date_granularity)) {
    return array();
  }

  // Convert the date to gregorian if needed.
  // We check this based on the value of $element['#date_type'] which defaults
  // to gregorian if not present.
  // To specify a default date in hijri, set $element['#date_type'] to hijri.
  $date = $element['#value'];
  if (!empty($date['date']) && (!isset($element['#date_type']) || $element['#date_type'] != 'hijri')) {
    $date = getdate(strtotime($date['date']));
    $date = taarikh_api_convert($date['year'], $date['mon'], $date['mday']);
    $date = taarikh_api_format($element['#date_format'], $date);
  }

  $settings = array(
    'changeMonth' => TRUE,
    'changeYear' => TRUE,
    'firstDay' => intval(variable_get('date_first_day', 0)),
    'showSpeed' => 'fast',
    'dateFormat' => taarikh_format_to_popup(date_popup_date_format($element), 'datepicker'),
    'yearRange' => $element['#date_year_range'],
    // Custom setting, will be expanded in Drupal.behaviors.date_popup().
    'fromTo' => FALSE,
  );

  // Create a unique id for each set of custom settings.
  $id = taarikh_js_settings_id($element['#id'], 'calendarpicker', $settings);

  // Manually build this element and set the value - this will prevent
  // corrupting the parent value.
  $parents = array_merge($element['#parents'], array('date'));
  $sub_element = array(
    '#type' => 'textfield',
    '#default_value' => $date,
    '#id' => $id,
    '#input' => FALSE,
    '#size' => !empty($element['#size']) ? $element['#size'] : 20,
    '#maxlength' => !empty($element['#maxlength']) ? $element['#maxlength'] : 30,
    '#attributes' => $element['#attributes'],
    '#parents' => $parents,
    '#name' => array_shift($parents) . '[' . implode('][', $parents) . ']',
    '#ajax' => !empty($element['#ajax']) ? $element['#ajax'] : FALSE,
  );
  $sub_element['#value'] = $sub_element['#default_value'];
  // TODO, figure out exactly when we want this description.
  // In many places it is not desired.
  $sub_element['#description'] = ' ' . t('E.g., @date', array('@date' => taarikh_api_format(date_popup_date_format($element))));

  return $sub_element;
}

/**
 * Validation function for date selector.
 *
 * When used as a Views widget, the validation step always gets triggered,
 * even with no form submission. Before form submission $element['#value']
 * contains a string, after submission it contains an array.
 */
function taarikh_select_validate($element, &$form_state) {
  if (date_hidden_element($element)) {
    return;
  }

  if (is_string($element['#value'])) {
    return;
  }

  $input_exists = NULL;
  $input = drupal_array_get_nested_value($form_state['values'], $element['#parents'], $input_exists);
  if (is_string($input)) {
    // The element is probably already validated and converted to gregorian.
    return;
  }

  // Strip field labels out of the results.
  foreach ($element['#value'] as $field => $field_value) {
    if (!$field_value || substr($field_value, 0, 1) == '-') {
      $input[$field] = '';
    }
  }

  $entered = array_values(array_filter($input));
  if (empty($entered) && !$element['#required']) {
    // Blank values are allowed.
    return;
  }

  $date = taarikh_select_input_date($element, $input);
  if (!taarikh_api_is_valid($date)) {
    form_error($element, t('Invalid hijri date specified for %name', array('%name' => $element['#instance']['label'])));
  }

  list($month, $day, $year) = explode("/", taarikh_api_convert_to_gregorian($date['year'], $date['month'], $date['day']));
  $date = date(DATE_FORMAT_DATE, mktime(0, 0, 0, $month, $day, $year));
  form_set_value($element, $date, $form_state);
}

/**
 * Massage the input values back into a single date.
 *
 * When used as a Views widget, the validation step always gets triggered,
 * even with no form submission. Before form submission $element['#value']
 * contains a string, after submission it contains an array.
 */
function taarikh_element_validate($element, &$form_state) {
  if (date_hidden_element($element)) {
    return;
  }

  if (is_string($element['#value'])) {
    return;
  }

  taarikh_add();

  $input_exists = NULL;
  $input = drupal_array_get_nested_value($form_state['values'], $element['#parents'], $input_exists);
  if (is_string($input)) {
    // The element is probably already validated and converted to gregorian.
    return;
  }

  if (!$input['date'] && !$element['#required']) {
    // Blank values are allowed.
    return;
  }

  $date = taarikh_input_date($element, $input);

  if (!isset($date['year']) || !isset($date['month']) || !isset($date['day'])) {
    form_error($element, t('Invalid hijri date specified for %name', array('%name' => $element['#instance']['label'])));
  }

  list($month, $day, $year) = explode("/", taarikh_api_convert_to_gregorian($date['year'], $date['month'], $date['day']));
  $date = date(DATE_FORMAT_DATE, mktime(0, 0, 0, $month, $day, $year));
  form_set_value($element, $date, $form_state);
}

/**
 * Creates form elements for one or more date parts.
 *
 * Get the order of date elements from the provided format.
 * If the format order omits any date parts in the granularity, alter the
 * granularity array to match the format, then flip the $order array
 * to get the position for each element. Then iterate through the
 * elements and create a sub-form for each part.
 *
 * @param array $element
 *   The date element.
 * @param array $date
 *   The date object.
 * @param string $format
 *   A date format string.
 *
 * @return array
 *   The form array for the submitted date parts.
 */
function taarikh_parts_element(array $element, array $date, $format) {
  $granularity = date_format_order($format);
  $sub_element = array('#granularity' => $granularity);
  $order = array_flip($granularity);
  $element += array('#date_text_parts' => array());

  $hours_format  = strpos(strtolower($element['#date_format']), 'a') ? 'g' : 'G';

  // Allow empty value as option if date is not required or there is no date.
  $part_required = (bool) $element['#required'] && is_array($date);
  foreach ($granularity as $field) {
    $part_type = in_array($field, $element['#date_text_parts']) ? 'textfield' : 'select';
    $sub_element[$field] = array(
      '#weight' => $order[$field],
      '#required' => $part_required,
      '#attributes' => array('class' => isset($element['#attributes']['class']) ? $element['#attributes']['class'] += array('date-' . $field) : array('date-' . $field)),
      '#ajax' => !empty($element['#ajax']) ? $element['#ajax'] : FALSE,
    );
    switch ($field) {
      case 'year':
        $range = taarikh_range_years($element['#date_year_range'], $date['year']);
        $min_year = $range[0];
        $max_year = $range[1];

        $sub_element[$field]['#default_value'] = isset($date['year']) ? $date['year'] : '';
        if ($part_type == 'select') {
          $sub_element[$field]['#options'] = drupal_map_assoc(date_years($min_year, $max_year, $part_required));
        }
        break;

      case 'month':
        $sub_element[$field]['#default_value'] = isset($date['month']) ? $date['month'] : '';
        if ($part_type == 'select') {
          $months = array();
          $long_month = strpos($element['#date_format'], 'F') !== FALSE;
          if (!$part_required) {
            $months[0] = '';
          }
          for ($i = 1; $i <= 12; $i++) {
            $months[$i] = taarikh_api_month($i, $long_month ? 'long' : 'short');
          }
          $sub_element[$field]['#options'] = $months;
        }
        break;

      case 'day':
        $sub_element[$field]['#default_value'] = isset($date['day']) ? $date['day'] : '';
        if ($part_type == 'select') {
          $days = array();
          if (!$part_required) {
            $days[0] = '';
          }
          $days = array_merge($days, range(1, 30));
          $sub_element[$field]['#options'] = drupal_map_assoc($days);
        }
        break;

      case 'hour':
        $sub_element[$field]['#default_value'] = is_object($date) ? $date->format($hours_format) : '';
        if ($part_type == 'select') {
          $sub_element[$field]['#options'] = drupal_map_assoc(date_hours($hours_format, $part_required));
        }
        $sub_element[$field]['#prefix'] = theme('date_part_hour_prefix', $element);
        break;

      case 'minute':
        $sub_element[$field]['#default_value'] = is_object($date) ? $date->format('i') : '';
        if ($part_type == 'select') {
          $sub_element[$field]['#options'] = drupal_map_assoc(date_minutes('i', $part_required, $element['#date_increment']));
        }
        $sub_element[$field]['#prefix'] = theme('date_part_minsec_prefix', $element);
        break;

      case 'second':
        $sub_element[$field]['#default_value'] = is_object($date) ? $date->format('s') : '';
        if ($part_type == 'select') {
          $sub_element[$field]['#options'] = drupal_map_assoc(date_seconds('s', $part_required, $element['#date_increment']));
        }
        $sub_element[$field]['#prefix'] = theme('date_part_minsec_prefix', $element);
        break;
    }

    // Add handling for the date part label.
    $label = theme('date_part_label_' . $field, array('part_type' => $part_type, 'element' => $element));
    if (in_array($field, $element['#date_text_parts'])) {
      $sub_element[$field]['#type'] = 'textfield';
      $sub_element[$field]['#theme'] = 'date_textfield_element';
      $sub_element[$field]['#size'] = 7;
      $sub_element[$field]['#title'] = $label;
      $sub_element[$field]['#title_display'] = in_array($element['#date_label_position'], array('within', 'none')) ? 'invisible' : 'before';
      if ($element['#date_label_position'] == 'within') {
        if (!empty($sub_element[$field]['#options']) && is_array($sub_element[$field]['#options'])) {
          $sub_element[$field]['#options'] = array(
            '-' . $label => '-' . $label,
          ) + $sub_element[$field]['#options'];
        }
        if (empty($sub_element[$field]['#default_value'])) {
          $sub_element[$field]['#default_value'] = '-' . $label;
        }
      }
    }
    else {
      $sub_element[$field]['#type'] = 'select';
      $sub_element[$field]['#theme'] = 'date_select_element';
      $sub_element[$field]['#title'] = $label;
      $sub_element[$field]['#title_display'] = in_array($element['#date_label_position'], array('within', 'none')) ? 'invisible' : 'before';
      if ($element['#date_label_position'] == 'within') {
        $sub_element[$field]['#options'] = array(
          '' => '-' . $label,
        ) + $sub_element[$field]['#options'];
      }
    }
  }

  // Views exposed filters are treated as submitted even if not,
  // so force the #default value in that case. Make sure we set
  // a default that is in the option list.
  if (!empty($element['#force_value'])) {
    $options = $sub_element[$field]['#options'];
    $default = !empty($sub_element[$field]['#default_value']) ? $sub_element[$field]['#default_value'] : array_shift($options);
    $sub_element[$field]['#value'] = $default;
  }

  if (($hours_format == 'g' || $hours_format == 'h') && date_has_time($granularity)) {
    $label = theme('date_part_label_ampm', array('part_type' => 'ampm', 'element' => $element));
    $sub_element['ampm'] = array(
      '#type' => 'select',
      '#theme' => 'date_select_element',
      '#title' => $label,
      '#title_display' => in_array($element['#date_label_position'], array('within', 'none')) ? 'invisible' : 'before',
      '#default_value' => is_object($date) ? (date_format($date, 'G') >= 12 ? 'pm' : 'am') : '',
      '#options' => drupal_map_assoc(date_ampm($part_required)),
      '#required' => $part_required,
      '#weight' => 8,
      '#attributes' => array('class' => array('date-ampm')),
    );
    if ($element['#date_label_position'] == 'within') {
      $sub_element['ampm']['#options'] = array('' => '-' . $label) + $sub_element['ampm']['#options'];
    }
  }

  return $sub_element;
}

/**
 * Create a date object from a datetime string value.
 */
function taarikh_default_date($element) {
  $date = date_default_date($element);
  $year = $date->format('Y');
  $month = $date->format('n');
  $day = $date->format('j');

  $date = taarikh_api_convert($year, $month, $day);
  $taarikh['year'] = $date[0];
  $taarikh['month'] = $date[1];
  $taarikh['day'] = $date[2];

  return $taarikh;
}
