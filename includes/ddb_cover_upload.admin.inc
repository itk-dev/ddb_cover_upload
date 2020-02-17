<?php
/**
 * @file
 * Admin interface for handling uploads to cover service.
 */

define('DDB_COVER_UPLOAD_STEP1', 0);
define('DDB_COVER_UPLOAD_STEP2', 1);
define('DDB_COVER_UPLOAD_STEP3', 2);
define('DDB_COVER_UPLOAD_STEP4', 3);

/**
 * Returns the list of steps and their associated forms.
 *
 * @return array
 *   List of steps and their forms.
 */
function _ddb_cover_upload_form_steps() {
  return array(
    DDB_COVER_UPLOAD_STEP1 => array(
      'form' => 'ddb_cover_upload_conditions',
      'step_title' => t('First time use')
    ),
    DDB_COVER_UPLOAD_STEP2 => array(
      'form' => 'ddb_cover_upload_material',
      'step_title' => t('Select material')
    ),
    DDB_COVER_UPLOAD_STEP3 => array(
      'form' => 'ddb_cover_upload_image_upload',
      'step_title' => t('Select image')
    ),
    DDB_COVER_UPLOAD_STEP4 => array(
      'form' => 'ddb_cover_upload_meta',
      'step_title' => t('Verify image')
    ),
  );
}

/**
 * The primary form builder for the multi-step form.
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
 * @param array $form
 *   The form element.
 * @param array $form_state
 *   The state of the form.
 *
 * @return array
 *   The full form including all steps.
 */
function ddb_cover_upload_form(array $form, array &$form_state) {


  // Initialize a description of the steps for the wizard.
  if (empty($form_state['step'])) {
    _ddb_cover_upload_determine_step(drupal_get_query_parameters(), $form_state);

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
  $form['#attached'] = array (
    'css' => array (
      drupal_get_path('module', 'ddb_cover_upload') . '/css/ddb_cover_upload.css'
    ),
    'library' => array(
      array('system', 'ui.dialog'),
    ),
    'js' => array(
      drupal_get_path('module', 'ddb_cover_upload') . '/js/ddb_cover_upload_dialog.js',
      drupal_get_path('module', 'ddb_cover_upload') . '/js/ddb_cover_upload_crop.js'
    ),
    'libraries_load' => array(
      array('cropperjs'),
      array('jquery-cropper'),
      array('camanjs')
    ),
  );

  // Set title for step.
  drupal_set_title(_ddb_cover_upload_get_title($step, $form_state));

  // We use #limit_validation_errors to skip all complaints about validation
  // when using the back button. The values entered will be discarded, but they
  // will not be validated, which would be annoying in a "back" button.
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
 * Accept terms of use of the form.
 *
 * @param array $form
 *   The form element.
 * @param array $form_state
 *   The state of the form.
 *
 * @return array
 *   The form for displaying and accepting terms of use.
 *
 * @throws \Exception
 *   Theme exception error.
 */
function ddb_cover_upload_conditions(array $form, array &$form_state) {
  $form = array();
  $no_terms_set = t('[Contact DDB for copyright information and change module settings]');
  $terms = variable_get('ddb_cover_upload_terms_of_use')['value'];
  $form['ddb_cover_upload_conditions'] = array(
    '#markup' => theme('ddb_cover_upload_conditions', array(
      'copyright_text' => empty($terms) ? $no_terms_set : $terms
    ))
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
 * @param array $form
 *   The form element.
 * @param array $form_state
 *   The state of the form.
 *
 * @return array
 *   The form for referencing a material.
 */
function ddb_cover_upload_material(array $form, array &$form_state) {
  $form = array();
  // @todo Allow lookup through isbn etc.

  $form['ddb_cover_upload_material'] = array(
    '#type' => 'textfield',
    '#title' => t('Material'),
    '#description' => t('Hint: Lorem ipsum.'),
    '#autocomplete_path' => 'cover_upload/ting_reference/autocomplete',
    '#required' => TRUE,
    '#default_value' => !empty($form_state['values']['material']) ? $form_state['values']['material'] : '',
  );
  return $form;
}

/**
 * Image upload step of the form.
 *
 * @param array $form
 *   The form element.
 * @param array $form_state
 *   The state of the form.
 *
 * @return array
 *   The form for image upload.
 *
 * @throws \Exception
 *   Theme exception error.
 */
function ddb_cover_upload_image_upload(array $form, array &$form_state) {
  $form = array();
  // @todo Some how we need to handle users clicking Next before clicking fetch.
  $upload_validators = array(
    'file_validate_extensions' => array('png jpg jpeg'),
    'file_validate_size' => array(_ddb_cover_upload_return_bytes(ini_get('post_max_size'))),
  );

  $form['ddb_cover_upload_image'] = array(
    '#title' => t('Upload image'),
    '#type' => 'managed_file',
    '#required' => TRUE,
    '#default_value' => variable_get('ddb_cover_upload_image', ''),
    '#progress_indicator' =>'bar',
    '#progress_message' => 'Uploading...',
    '#upload_location' => 'public://upload_cover_service',
    '#upload_validators' => $upload_validators,
    '#description' => theme(
      'file_upload_help', array(
        'description' => t('Upload a file from your computer.'),
        'upload_validators' => $upload_validators
      )
    ),
  );

  $form['#suffix'] = theme('ddb_cover_upload_best_result', array());

  return $form;
}

/**
 * Meta data step of the form.
 *
 * @param array $form
 *   The form element.
 * @param array $form_state
 *   The state of the form.
 *
 * @return array
 *   The form for image upload.
 *
 * @throws \Exception
 *   Theme exception error.
 */
function ddb_cover_upload_meta(array $form, array &$form_state) {
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

  // The uploaded file.
  $uploaded = file_load($form_state['input']['ddb_cover_upload_image']['fid']);
  $publicPath = drupal_realpath(variable_get('file_public_path','sites/default/files'));

  // Determine full path for image.
  $originalImagePath = $publicPath . '/upload_cover_service/' . $uploaded->filename;

  // Get metadata for original image.
  $originalImage = getimagesize($originalImagePath);

  // Set name and path to use for altered image.
  $altImageName = $uploaded->fid . '.jpg';
  $alteredImagePath = $publicPath . '/upload_cover_service/' . $uploaded->fid . '/' . $altImageName;

  // Set default metadata when page is first loaded.
  $displayedImageUrl =  file_create_url($uploaded->uri);
  $displayedFileName = $uploaded->filename;
  $displayedWidth = $originalImage[0];
  $displayedHeight = $originalImage[1];
  $displayedType = $originalImage['mime'];
  $displayedSize = filesize($originalImagePath);
  $alteredImageTime = 0;
  $form_state['author'] = variable_get('site_name','undefined');
  $form_state['image_altered'] = FALSE;

  // If the image has been changed override variables.
  if(file_exists($alteredImagePath)) {
    $alteredImage = getimagesize($alteredImagePath);
    $displayedWidth = $alteredImage[0];
    $displayedHeight = $alteredImage[1];
    $displayedType = $alteredImage['mime'];
    $displayedSize = filesize($alteredImagePath);
    $displayedImageUrl = file_create_url(file_build_uri('/upload_cover_service/' . $uploaded->fid . '/' . $altImageName));
    // Use timestamp as query param on image to prevent caching
    // https://stackoverflow.com/questions/1077041/refresh-image-with-a-new-one-at-the-same-url/22429796#22429796
    $alteredImageTime = filemtime($alteredImagePath);
    $form_state['image_altered'] = TRUE;
  }

  $form['ddb_cover_upload_meta']['left']['ddb_cover_upload_meta_left'] = array(
    '#markup' => theme('ddb_cover_upload_meta_left', array(
        'original_image' => theme_image(array(
          'path' => $uploaded->uri,
          'attributes' => array (
            'id' => 'img-original',
            'data-file-id' => $uploaded->fid
          )
        )),
        'displayed_image' => $displayedImageUrl,
        'altered_time' => $alteredImageTime,
        'adapt_link_title' => $alteredImageTime > 0 ? t('Redo image adaption') : t('Adapt image'),
      )
    )
  );

  $form['ddb_cover_upload_meta']['right']['ddb_cover_upload_meta_data'] = array(
    '#markup' => theme('ddb_cover_upload_meta_data', array(
      'material' => ding_entity_load($form_state['step_information'][1]['stored_values']['ddb_cover_upload_material']),
      'meta_data' => array (
        'image_changed' => isset($alteredImage) ? TRUE : FALSE,
        'file_name' => $displayedFileName,
        'file_dimensions' => array ('x' => $displayedWidth, 'y' => $displayedHeight),
        'file_type' => $displayedType,
        'file_size' => $displayedSize,
        'author' => $form_state['author']
      ),
    )),
  );

  $form['ddb_cover_upload_meta']['right']['ddb_cover_upload_meta_input'] = array(
    '#type' => 'textfield',
    '#title' => t('Image accreditation'),
    '#default_value' => variable_get('ddb_cover_upload_meta_input', ''),
    '#size' => 60,
    '#maxlength' => 128,
  );

  $no_terms_set = t('[Contact DDB for copyright information and change module settings]');
  $terms = variable_get('ddb_cover_upload_terms_of_use')['value'];
  $form['ddb_cover_upload_meta']['right']['ddb_cover_upload_meta_copyright'] = array(
    '#markup' => theme('ddb_cover_upload_meta_copyright', array(
      'copyright_text' => empty($terms) ? $no_terms_set : $terms
    ))
  );

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
 * @param array $form
 *   The form element.
 * @param array $form_state
 *   The state of the form.
 */
function ddb_cover_upload_previous_submit(array $form, array &$form_state) {
  $current_step = &$form_state['step'];
  $form_state['step_information'][$current_step]['stored_values'] = $form_state['input'];
  if ($current_step > DDB_COVER_UPLOAD_STEP2) {
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
 * @param array $form
 *   The form element.
 * @param array $form_state
 *   The state of the form.
 */
function ddb_cover_upload_next_submit(array $form, array &$form_state) {
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
 * Multi step form submit handler for cover upload.
 *
 * @param array $form
 *   The form element.
 * @param array $form_state
 *   The state of the form.
 *
 * @throws \Exception
 *   Watchdog exception error.
 */
function ddb_cover_upload_form_submit(array $form, array &$form_state) {
  $current_step = &$form_state['step'];
  $form_state['step_information'][$current_step]['stored_values'] = $form_state['values'];
  $form_state['redirect'] = '/admin/config/cover_upload/submitted';

  _ddb_cover_upload_save_conditions_acceptance();

  $image_data = array(
    'pid' => $form_state['step_information'][1]['stored_values']['ddb_cover_upload_material'],
    'fid' => $form_state['step_information'][2]['stored_values']['ddb_cover_upload_image'],
    'accreditation' => $form_state['input']['ddb_cover_upload_meta_input'],
    'author' => $form_state['author'],
    'image_altered' => $form_state['image_altered']
  );

  _ddb_cover_upload_session('ddb_cover_upload_submitted', $image_data);
}

/**
 * Determine starting step when opening form.
 *
 * @param array $query
 *   The query parameters.
 * @param array $form_state
 *   The state of the form.
 */
function _ddb_cover_upload_determine_step(array $query, array &$form_state) {
  $form_state['step'] = DDB_COVER_UPLOAD_STEP1;

  if (_ddb_cover_upload_is_conditions_accepted()) {
    $form_state['step'] = DDB_COVER_UPLOAD_STEP2;

    // Also bypass material selection if pid is included as query parameter.
    if(isset($query['pid'])) {
      $form_state['step'] = DDB_COVER_UPLOAD_STEP3;
    }
  }
}

/**
 * Check if the state of conditions of usages acceptance.
 *
 * @return bool
 *   TRUE if they have been accepted else FALSE.
 */
function _ddb_cover_upload_is_conditions_accepted() {
  global $user;

  return isset($user->data['ddb_cover_upload_conditions_accepted']) ?
    $user['current_user']->data['ddb_cover_upload_conditions_accepted'] :
    FALSE;
}

/**
 * Save usage acceptance.
 *
 * @param bool $value
 *   If TRUE it is accepted else FALSE. Default TRUE.
 *
 * @return bool
 *   TRUE if saved else FALSE.
 */
function _ddb_cover_upload_save_conditions_acceptance(bool $value = TRUE) {
  global $user;

  try {
    $user->data['ddb_cover_upload_conditions_accepted'] = TRUE;
    user_save($user);

    return TRUE;
  }
  catch (Exception $exception) {
    drupal_set_message(t('Error saving user acceptance of usage conditions'), 'error');
    watchdog_exception('ddb_cover_upload', $exception);

    RETURN FALSE;
  }
}

/**
 * Get a title for the form step.
 *
 * @param int $step
 *   Display title depending on steps.
 * @param array $form_state
 *   The state of the form.
 * @return array|mixed|string|null
 *   The title to display.
 */
function _ddb_cover_upload_get_title(int $step, array $form_state) {
  if ($step == DDB_COVER_UPLOAD_STEP1) {
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
 * @param string $val
 *   A string representation of file sizes.
 *
 * @return float|int|string
 *   A calculated value of bytes.
 */
function _ddb_cover_upload_return_bytes(string $val) {
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

