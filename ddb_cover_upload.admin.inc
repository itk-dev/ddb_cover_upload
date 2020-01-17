<?php

/**
 * @file
 * Admin interface for handling uploads to cover service.
 */

/**
 * @todo Remove these comments.
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
 * @return array
 *   List of steps and their forms.
 *
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
 * The primary formbuilder for the multistep form.
 *
 * This will include the rest of the form steps defined.
 *
 * This form has two defined submit handlers to process the different steps:
 *  - Previous: handles the way to get back one step in the wizard.
 *  - Next:     handles each step form submission,
 *
 * The third handler, the finish button handler, is the default form_submit
 * handler used to process the information.
 *
 * @param $form
 *   The form element.
 * @param $form_state
 *   The state of the form.
 *
 * @return mixed
 *   The full form including all steps.
 */
function ddb_cover_upload_form($form, &$form_state) {
  global $user;
  // Initialize a description of the steps for the wizard.
  if (empty($form_state['step'])) {
    _ddb_cover_upload_determine_step($user, drupal_get_query_parameters(),$form_state);

    // This array contains the function to be called at each step to get the
    // relevant form elements. It will also store state information for each
    // step.
    $form_state['step_information'] = _ddb_cover_upload_form_steps();
  }
  $step = &$form_state['step'];

  // Call the function named in $form_state['step_information'] to get the
  // form elements to display for this step.
  $form = $form_state['step_information'][$step]['form']($form, $form_state);
  // Attach assets to form.
  $form['#attached']['css'][] = drupal_get_path('module', 'ddb_cover_upload') . '/css/ddb_cover_upload.css';

  // Set title for step.
  drupal_set_title(_ddb_cover_upload_get_title($step, $form_state));

  // We use #limit_validation_errors to
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
    $form['next'] = array(
      '#type' => 'submit',
      '#value' => t('Next'),
      '#name' => 'next',
      '#submit' => array('ddb_cover_upload_next_submit'),
    );
  }
  else {
    $form['finish'] = array(
      '#type' => 'submit',
      '#value' => t('Upload image to cover service'),
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
 * @param $form
 *   The form element.
 * @param $form_state
 *   The state of the form.
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
 * @param $form
 *   The form element.
 * @param $form_state
 *   The state of the form.
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
 * Accept terms of use of the form.
 *
 * @param $form
 *   The form element.
 * @param $form_state
 *   The state of the form.
 *
 * @return array
 *   The form for displaying and accepting terms of use.
 *
 * @throws \Exception
 *   Theme exception error.
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
 * Select ting material step of the form.
 *
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
 * Validation for "Select ting material" step.
 *
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
 * Image upload step of the form.
 *
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
 * Meta data step of the form.
 *
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
  // Add uploaded file to template.
  $uploaded = file_load($form_state['input']['ddb_cover_upload_image']['fid']);
  $form['ddb_cover_upload_meta']['left']['ddb_cover_upload_meta_left'] = array(
    '#markup' => theme('ddb_cover_upload_meta_left', array(
        'image' => theme('image_style', array(
          'style_name' => 'large',
          'path' => $uploaded->uri,
          'attributes' => array (
            'class' => 'img-responsive',
          )
        ))
      )
    )
  );
  $form['ddb_cover_upload_meta']['right']['ddb_cover_upload_meta_data'] = array(
    '#markup' => theme('ddb_cover_upload_meta_data', array(
      'file_data' => $uploaded,
      'meta_data' => array (
        'file_size' => array ('x' => 500, 'y' => 1000),
        'author' => 'Aarhus Kommunes Bibilioteker',
      ),
    )),
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
 * Multistep form submit handler for cover upload.
 *
 * @param $form
 *   The form element.
 * @param $form_state
 *   The state of the form.
 */
function ddb_cover_upload_form_submit($form, &$form_state) {
  $current_step = &$form_state['step'];
  $form_state['step_information'][$current_step]['stored_values'] = $form_state['values'];
  $form_state['redirect'] = '/admin/config/cover_upload/submitted';

  // @todo Call upload service.
}

/**
 * Determine starting step when opening form.
 *
 * @param $user
 *   The active user.
 * @param $query
 *   The query parameters.
 * @param $form_state
 *   The state of the form.
 */
function _ddb_cover_upload_determine_step($user, $query, &$form_state) {
  $form_state['step'] = 0;
  // @todo Bypass step 1 if user has already accepted conditions.
  if ($user) {
    $form_state['step'] = 1;

    // Also bypass material selection if pid is included as query parameter.
    if(isset($query['pid'])) {
      $form_state['step'] = 2;
    }
  }
  // @todo remove this.
  $form_state['step'] = 0;
}

/**
 * Get a title for the form step.
 *
 * @param $step
 *   Display title depending on steps.
 * @param $form_state
 *   The state of the form.
 * @return array|mixed|string|null
 *   The title to display.
 */
function _ddb_cover_upload_get_title($step, $form_state) {
  if ($step == 0) {
    $title = t('Upload cover: @step_title', array('@step_title' => $form_state['step_information'][$step]['step_title']));
  }
  else {
    $title = t('Upload cover: Step @step - @step_title', array('@step' => $step, '@step_title' => $form_state['step_information'][$step]['step_title']));
  }
  return $title;
}

/**
 * Translate file size values from strings calculated bytes.
 *
 * @param $val
 *   A string representation of file sizes.
 *
 * @return float|int|string
 *   A calculated value of bytes.
 */
function _ddb_cover_upload_return_bytes($val) {
  $val = trim($val);
  $last = strtolower($val[strlen($val)-1]);
  $val = intval($val);
  switch($last) {
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

/**
 * Display cover submitted page.
 *
 * @return array
 *   A themed page.
 * @throws \Exception
 *   Theme exception error.
 */
function ddb_cover_upload_submitted() {
  return array(
    '#attached' => array (
      'css' => array(
        drupal_get_path('module', 'ddb_cover_upload') . '/css/ddb_cover_upload.css',
      ),
    ),
    '#markup' => theme('ddb_cover_upload_submitted', array())
  );
}

/**
 * Display list of covers uploaded by the library.
 *
 * @return array
 *   A themed list.
 * @throws \Exception
 *   Theme exception error.
 */
function ddb_cover_upload_list() {
  drupal_set_title(t('List uploaded covers'));
  $header = array(
    t('Image'),
    t('Material'),
    t('Actions')
  );
  $rows = array();
  return array(
    '#markup' => theme('table', array(
      'header' => $header,
      'rows' => $rows,
    ))
  );
}
