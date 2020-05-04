<?php
/**
 * @file
 * Cover upload page when form is submitted.
 */

/**
 * Display cover submitted page.
 *
 * @return array
 *   A themed page.
 * @throws \Exception
 *   Theme exception error.
 */
function ddb_cover_upload_submitted() {
  $data = _ddb_cover_upload_session('ddb_cover_upload_submitted');
  $image_url = file_create_url($data['image_uri']);

  return array(
    '#attached' => array (
      'css' => array(
        drupal_get_path('module', 'ddb_cover_upload') . '/css/ddb_cover_upload.admin.css',
      ),
      'js' => array(
        drupal_get_path('module', 'ddb_cover_upload') . '/js/ddb_cover_upload_check_submission.admin.js',
      ),
    ),
    '#markup' => theme('ddb_cover_upload_submitted', array(
      'data' => $data,
      'image' => $image_url,
    ))
  );
}

/**
 * Check cover service to determine if file has been uploaded.
 */
function _cover_upload_check_submission() {
  $data = _ddb_cover_upload_session('ddb_cover_upload_submitted');

  try {
    _ddb_cover_upload_client_upload($data['image_uri'], $data['pid'], 'pid');
  }
  catch (\CoverService\ApiException $e) {
    // @TODO: handle exception
    drupal_json_output(array(
      'message' => $e->getMessage(),
      'status' => 'error',
    ));
    drupal_exit();
  }

  // @TODO clean up.

  // Notify frontend "Nothing to see yet".
  drupal_json_output($state = array(
    'status' => 'success',
  ));
  drupal_exit();
}

/**
 * Remove db entry and files locally.
 *
 * @param array $data
 *   The ddb_cover_upload data from session.
 */
function _ddb_cover_upload_cleanup(array $data) {

}

/**
 * Wrapper to upload a cover to the service.
 *
 * @param string $image_uri
 *   The local URI for the cover image.
 * @param string $identifier
 *   The identifier for the material for the cover.
 * @param string $identifier_type
 *   The type of identifier (faust, pid, isbn).
 *
 * @throws \CoverService\ApiException
 */
function _ddb_cover_upload_client_upload(string $image_uri, string $identifier, string $identifier_type) {
  require_once(__DIR__ . '/../vendor/autoload.php');

  $image = drupal_realpath($image_uri);
  $config = _ddb_cover_upload_client_config();
  $apiInstance = new CoverService\Api\CoverApi(
    new GuzzleHttp\Client(),
    $config
  );

  /** @var \CoverService\Model\Cover $cover */
  $cover = $apiInstance->postCoverCollection($image);

  $material = new \CoverService\Model\MaterialWrite();
  $material->setCover('/api/covers/'.$cover->getId());
  $material->setIsType($identifier_type);
  $material->setIsIdentifier($identifier);

  $apiInstance = new CoverService\Api\MaterialApi(
    new GuzzleHttp\Client(),
    $config
  );
  $apiInstance->postMaterialCollection($material);
}

/**
 * Helper to get configuration for the upload API.
 *
 * @return \CoverService\Configuration
 *   API configuration.
 *
 * @throws \DingProviderDoesntImplement
 * @throws \DingProviderNoProvider
 */
function _ddb_cover_upload_client_config() {
  $config = CoverService\Configuration::getDefaultConfiguration();

  // Set token from current session.
  // @TODO: This would be for the user an not the site.
  $token = _ddb_cover_upload_get_library_token();
  $config->setAccessToken($token);

  $config->setHost(variable_get('ddb_cover_upload_host', 'https://upload.cover.dandigbib.org'));

  return $config;
}
