<?php
  
/**
 * @file
 * Contains \Drupal\taarikh\Plugin\field\widget\TaarikhWidget.
 */
   
namespace Drupal\taarikh\Plugin\Field\FieldWidget;
  
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Field\WidgetBase;
use Drupal\Core\Form\FormStateInterface;
use \Drupal\datetime\Plugin\Field\FieldWidget\DateTimeDefaultWidget;
use Drupal\Core\Datetime\DrupalDateTime;  
  
  
/**
 * @FieldWidget(
 *   id = "Popup_Calendar_Hijri",
 *   module = "taarikh",
 *   label = @Translation("Popup Calendar Hijri"),
 *   field_types = {
 *     "datetime"
 *   }
 * )
 */
class Popup_Calendar_Hijri_Widget extends DateTimeDefaultWidget {
  
 /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {
    $element = parent::formElement($items, $delta, $element, $form, $form_state);
    $form['#attached']['library'][] = 'taarikh/taarikh';
      
    if($element['value']['#default_value'] != NULL){
    $hijri_date = taarikh_api_convert(
        $element['value']['#default_value']->format('Y'), 
        $element['value']['#default_value']->format('m'),
        $element['value']['#default_value']->format('d'));
          
    $element['value']['#default_value'] = new DrupalDateTime(implode('-', $hijri_date));
    }
    $element['#element_validate'][0] =  array($this, 'validate');
    return $element;
  }
    
  /**
   * Validate the color text field.
   */
  public function validate($element, FormStateInterface $form_state) {
    $date = explode("-", $element['value']['#value']['date']);
    $gregorian_date = taarikh_api_convert_to_gregorian($date[0], $date[1], $date[2]);
    
    $form_state->getValues()['field_dat'][0]['value'] = $element['value']['#value']['object'] = new DrupalDateTime($gregorian_date);
    $element['value']['#value']['date'] = str_replace('/', '-', $gregorian_date);

    return true;
  }
  }
