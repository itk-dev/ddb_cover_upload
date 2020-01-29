<?php
/**
 * @file
 * Cover upload settings form.
 */

/**
 * Cover upload settings form.
 *
 * @param array $form
 *   Form structure.
 * @param array $form_state
 *   Form state values.
 *
 * @return array
 *   Form structure for admin settings.
 */
function ddb_cover_upload_settings_form($form, $form_state) {
  $form['settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Cover upload settings'),
  );

  $form['settings']['ddb_cover_upload_terms_of_use'] = array(
    '#type' => 'text_format',
    '#title' => t('Terms of use'),
    '#default_value' => variable_get('ddb_cover_upload_terms_of_use')['value'],
    '#text_format' => 'ding_wysiwyg',
  );

  return system_settings_form($form);
}
