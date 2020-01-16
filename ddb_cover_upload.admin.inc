<?php

/**
 * @file
 * Upload cover form.
 */

/**
 * Extensible wizard form example.
 *
 * This is an example of a multistep form using a wizard style. It will include
 * the 'Previous' and 'Next' buttons when required, and a 'Finish' button at the
 * last stage of the form submission.
 *
 * This example is an extensible skeleton that can include (even
 * programmatically) more steps. The demonstration form includes three steps,
 * each step having its own validation functions.
 *
 * How to extend this example:
 * - Steps are defined in the _form_example_steps() function. Include or alter
 *   the steps as you require.
 * - For each step, implement the corresponding 'form' function (see
 *   'ddb_cover_upload_conditions' for the first step in this example.)
 *   Each step is a regular form, and the wizard collects all the values of the
 *   included forms.
 * - Optionally, you may include custom validation functions using the regular
 *   validation hook (formname_validate). The wizard uses these validation
 *   functions for each step.
 * - The most important customization step is to change the submit handler and
 *   do whatever you want with the collected information. In this case, the
 *   example just shows the collected values in the various steps.
 * @ingroup form_example
 */

/**
 * Returns the list of steps and their associated forms.
 *
 * This has been separated to clarify and easy the understanding of this
 * example. You should edit this function to include the steps your
 * wizard/multistep form requires.
 *
 * @return array
 *   List of steps and their forms.
 *
 * @ingroup form_example
 */
function _ddb_cover_upload_form_steps() {
  return array(
    0 => array(
      'form' => 'ddb_cover_upload_conditions',
      'step_title' => t('First time use')
    ),
    1 => array(
      'form' => 'ddb_cover_upload_material',
      'step_title' => t('Select material')
    ),
    2 => array(
      'form' => 'ddb_cover_upload_image_upload',
      'step_title' => t('Select image')
    ),
    3 => array(
      'form' => 'ddb_cover_upload_meta',
      'step_title' => t('Verify image')
    ),
  );
}

/**
 * The primary formbuilder function for the wizard form.
 *
 * This is the form that you should call with drupal_get_form() from your code,
 * and it will include the rest of the step forms defined. You are not required
 * to change this function, as this will handle all the step actions for you.
 *
 * This form has two defined submit handlers to process the different steps:
 *  - Previous: handles the way to get back one step in the wizard.
 *  - Next:     handles each step form submission,
 *
 * The third handler, the finish button handler, is the default form_submit
 * handler used to process the information.
 *
 * You are not required to change the next or previous handlers, but you must
 * change the ddb_cover_upload_submit handler to perform the operations you
 * need on the collected information.
 *
 * @ingroup form_example
 */
function ddb_cover_upload_form($form, &$form_state) {
  global $user;
  // Initialize a description of the steps for the wizard.
  if (empty($form_state['step'])) {
    _ddb_cover_upload_define_step($user, drupal_get_query_parameters(),$form_state);

    // This array contains the function to be called at each step to get the
    // relevant form elements. It will also store state information for each
    // step.
    $form_state['step_information'] = _ddb_cover_upload_form_steps();
  }
  $step = &$form_state['step'];

  // Call the function named in $form_state['step_information'] to get the
  // form elements to display for this step.
  $form = $form_state['step_information'][$step]['form']($form, $form_state);
  drupal_set_title(_ddb_cover_upload_get_title($step, $form_state));
  //drupal_set_title(t('Upload cover: Step @step - @step_title', array('@step' => $step, '@step_title' => $form_state['step_information'][$step]['step_title'])));

  // Show the 'previous' button if appropriate. Note that #submit is set to
  // a special submit handler, and that we use #limit_validation_errors to
  // skip all complaints about validation when using the back button. The
  // values entered will be discarded, but they will not be validated, which
  // would be annoying in a "back" button.
  if ($step > 1) {
    $form['prev'] = array(
      '#type' => 'submit',
      '#value' => t('Previous'),
      '#name' => 'prev',
      '#submit' => array('ddb_cover_upload_previous_submit'),
      '#limit_validation_errors' => array(),
    );
  }

  // Show the Next button only if there are more steps defined.
  if ($step < count($form_state['step_information']) -1) {
    // The Next button should be included on every step.
    $form['next'] = array(
      '#type' => 'submit',
      '#value' => t('Next'),
      '#name' => 'next',
      '#submit' => array('ddb_cover_upload_next_submit'),
    );
  }
  else {
    // Just in case there are no more steps, we use the default submit handler
    // of the form wizard. Call this button Finish, Submit, or whatever you
    // want to show. When this button is clicked, the
    // ddb_cover_upload_submit handler will be called.
    $form['finish'] = array(
      '#type' => 'submit',
      '#value' => t('Add image'),
    );
  }

  // Include each validation function defined for the different steps.
  if (function_exists($form_state['step_information'][$step]['form'] . '_validate')) {
    $form['next']['#validate'] = array($form_state['step_information'][$step]['form'] . '_validate');
  }

  return $form;
}

/**
 * Submit handler for the "previous" button.
 *
 * This function:
 * - Stores away $form_state['values']
 * - Decrements the step counter
 * - Replaces $form_state['values'] with the values from the previous state.
 * - Forces form rebuild.
 *
 * You are not required to change this function.
 *
 * @ingroup form_example
 */
function ddb_cover_upload_previous_submit($form, &$form_state) {
  $current_step = &$form_state['step'];
  $form_state['step_information'][$current_step]['stored_values'] = $form_state['input'];
  if ($current_step > 1) {
    $current_step--;
    if(isset($form_state['step_information'][$current_step]['stored_values'])) {
      $form_state['values'] = $form_state['step_information'][$current_step]['stored_values'];
    }
  }
  $form_state['rebuild'] = TRUE;
}

/**
 * Submit handler for the 'next' button.
 *
 * This function:
 * - Saves away $form_state['values']
 * - Increments the step count.
 * - Replace $form_state['values'] from the last time we were at this page
 *   or with array() if we haven't been here before.
 * - Force form rebuild.
 *
 * You are not required to change this function.
 *
 * @ingroup form_example
 */
function ddb_cover_upload_next_submit($form, &$form_state) {
  $current_step = &$form_state['step'];
  $form_state['step_information'][$current_step]['stored_values'] = $form_state['values'];

  if ($current_step < count($form_state['step_information'])) {
    $current_step++;
    if (!empty($form_state['step_information'][$current_step]['stored_values'])) {
      $form_state['values'] = $form_state['step_information'][$current_step]['stored_values'];
    }
    else {
      $form_state['values'] = array();
    }
    // Force rebuild with next step.
    $form_state['rebuild'] = TRUE;
    return;
  }
}

/**
 * @param $form
 * @param $form_state
 *
 * @return array
 * @throws \Exception
 */
function ddb_cover_upload_conditions($form, &$form_state) {
  $form = array();
  $form['ddb_cover_upload_conditions'] = array(
    '#markup' => theme('ddb_cover_upload_conditions', array())
  );

  $form['ddb_cover_upload_confirm'] = array(
    '#type' => 'checkbox',
    '#title' => t('I accept conditions'),
    '#required' => TRUE,
  );
  return $form;
}

/**
 * @param $form
 *   The form element.
 * @param $form_state
 *   The state of the form.
 *
 * @return array
 *   The form for referencing a material.
 */
function ddb_cover_upload_material($form, &$form_state) {
  $form = array();
  // @todo Change this to a ting reference field.
  $form['ddb_cover_upload_material'] = array(
    '#type' => 'textfield',
    '#title' => t('Material'),
    '#description' => t('Hint: Lorem ipsum.'),
    '#required' => TRUE,
    '#default_value' => !empty($form_state['values']['material']) ? $form_state['values']['material'] : '',
  );
  return $form;
}

/**
 * @param $form
 *   The form element.
 * @param $form_state
 *   The state of the form.
 */
function ddb_cover_upload_material_validate($form, &$form_state) {
  // We must render the title again.
  $step = &$form_state['step'];
  drupal_set_title(_ddb_cover_upload_get_title($step, $form_state));
}

/**
 * @param $form
 *   The form element.
 * @param $form_state
 *   The state of the form.
 *
 * @return array
 *   The form for image upload.
 *
 * @throws \Exception
 *   Theme exception error.
 */
function ddb_cover_upload_image_upload($form, &$form_state) {
  $form = array();

  $upload_validators = array(
    'file_validate_extensions' => array('gif png jpg jpeg'),
    'file_validate_size' => array(_ddb_cover_upload_return_bytes(ini_get('post_max_size'))),
  );
  $form['ddb_cover_upload_image'] = array(
    '#title' => t('Upload image'),
    '#type' => 'managed_file',
    '#required' => TRUE,
    '#default_value' => variable_get('ddb_cover_upload_image', ''),
    '#progress_indicator' =>'bar',
    '#progress_message' => 'Uploading...',
    '#upload_location' => 'public://',
    '#upload_validators' =>$upload_validators,
    '#description' => theme('file_upload_help', array('description' => 'Upload a file from your computer.', 'upload_validators' => $upload_validators)),
  );

  $form['#suffix'] = theme('ddb_cover_upload_best_result', array());

  return $form;
}


/**
 * @param $form
 *   The form element.
 * @param $form_state
 *   The state of the form.
 *
 * @return array
 *   The form for image upload.
 *
 * @throws \Exception
 *   Theme exception error.
 */
function ddb_cover_upload_meta($form, &$form_state) {
  $form = array();
  $form['ddb_cover_upload_meta'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('wrapper', 'admin', 'clearfix')
    ),
  );
  $form['ddb_cover_upload_meta']['left'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('left')
    ),
  );
  $form['ddb_cover_upload_meta']['right'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('right')
    ),
  );
  $form['ddb_cover_upload_meta']['left']['ddb_cover_upload_meta_left'] = array(
    '#markup' => theme('ddb_cover_upload_meta_left', array())
  );
  $form['ddb_cover_upload_meta']['right']['ddb_cover_upload_meta_data'] = array(
    '#markup' => theme('ddb_cover_upload_meta_data', array())
  );
  $form['ddb_cover_upload_meta']['right']['ddb_cover_upload_meta_input'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject'),
    '#default_value' => variable_get('ddb_cover_upload_meta_input', ''),
    '#size' => 60,
    '#maxlength' => 128,
  );
  $form['ddb_cover_upload_meta']['right']['ddb_cover_upload_meta_copyright'] = array(
    '#markup' => theme('ddb_cover_upload_meta_copyright', array())
  );

  return $form;
}

/**
 * Wizard form submit handler.
 *
 * This function:
 * - Saves away $form_state['values']
 * - Process all the form values.
 *
 * And now comes the magic of the wizard, the function that should handle all
 * the inputs from the user on each different step.
 *
 * This demonstration handler just do a drupal_set_message() with the
 * information collected on each different step of the wizard.
 *
 * @ingroup form_example
 */
function ddb_cover_upload_form_submit($form, &$form_state) {
  $current_step = &$form_state['step'];
  $form_state['step_information'][$current_step]['stored_values'] = $form_state['values'];
  $form_state['redirect'] = '/admin/config/cover_upload/submitted';
  // In this case we've completed the final page of the wizard, so process the
  // submitted information.
  drupal_set_message(t('This information was collected by this wizard:'));
  foreach ($form_state['step_information'] as $index => $value) {
    // Remove FAPI fields included in the values (form_token, form_id and
    // form_build_id. This is not required, you may access the values using
    // $value['stored_values'] but I'm removing them to make a more clear
    // representation of the collected information as the complete array will
    // be passed through drupal_set_message().
    unset($value['stored_values']['form_id']);
    unset($value['stored_values']['form_build_id']);
    unset($value['stored_values']['form_token']);

    // Now show all the values.
    drupal_set_message(t('Step @num collected the following values: <pre>@result</pre>',
      array(
        '@num' => $index,
        '@result' => print_r($value['stored_values'], TRUE),
      )
    ));
  }
}

function _ddb_cover_upload_define_step($user, $query, &$form_state) {
  $form_state['step'] = 0;
  // @todo Bypass step 1 if user has already accepted conditions.
  if ($user) {
    $form_state['step'] = 1;

    // Also bypass material selection if pid is included as query parameter.
    if(isset($query['pid'])) {
      $form_state['step'] = 2;
    }
  }
  $form_state['step'] = 0;
}

function _ddb_cover_upload_get_title($step, $form_state) {
  if ($step == 0) {
    $title = t('Upload cover: @step_title', array('@step_title' => $form_state['step_information'][$step]['step_title']));
  }
  else {
    $title = t('Upload cover: Step @step - @step_title', array('@step' => $step, '@step_title' => $form_state['step_information'][$step]['step_title']));
  }
  return $title;
}

function _ddb_cover_upload_return_bytes($val) {
  $val = trim($val);
  $last = strtolower($val[strlen($val)-1]);
  $val = intval($val);
  switch($last) {
    // The 'G' modifier is available since PHP 5.1.0
    case 'g':
      $val *= 1024*1024*1024;
      break;

    case 'm':
      $val *= 1024*1024;
      break;

    case 'k':
      $val *= 1024;
      break;
  }

  return $val;
}

function ddb_cover_upload_submitted() {
  return array(
    '#markup' => theme('ddb_cover_upload_submitted', array())
  );
}
