<?php
/**
 * @file
 * List display of uploaded covers from this library.
 */

/**
 * Display list of covers uploaded by the library.
 *
 * @return array
 *   A themed list.
 * @throws \Exception
 *   Theme exception error.
 */
function ddb_cover_upload_list() {
  $rows = array();

  $materials = _ddb_cover_upload_get_materials();
  foreach ($materials as $material) {
    $cover_ref = $material->getCover();
    $id = _ddb_cover_upload_parse_cover_id($cover_ref);
    $cover = _ddb_cover_upload_get_cover($id);
    $image = array(
      '#theme' => 'image',
      '#path' => $cover->getImageUrl(),
      '#width' => '120px',
    );

    /** @var \Ting\TingObject $object */
    $object = ding_entity_load($material->getIsIdentifier(), 'ting_object');
    $uri = ding_entity_uri('ding_entity', $object);

    $rows[] = [
      $material->getId(),
      render($image),
      $object->getTitle() . ' (' . $object->getType() .')',
      l($material->getIsIdentifier(), $uri['path'], array('html' => TRUE)),
      l(t('Delete'), 'admin/config/cover_upload/upload/list/delete/' . $material->getId(), array('html' => TRUE)),
    ];
  }

  drupal_set_title(t('List uploaded covers'));
  $header = array(
    t('Id'),
    t('Image'),
    t('Title'),
    t('Material'),
    t('Actions')
  );

  return array(
    '#markup' => theme('table', array(
      'header' => $header,
      'rows' => $rows,
    ))
  );
}


/**
 * Menu callback for deleting an uploaded cover.
 */
function ddb_cover_upload_delete($form, &$form_state, $id) {
  $form['material_id'] = array(
    '#type' => 'value',
    '#value' => $id,
  );

  return confirm_form(
    $form,
    t('Are you sure you want to delete this item?'),
    'admin/config/cover_upload/upload/list',
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Confirm form submit handler for delete above.
 */
function ddb_cover_upload_delete_submit($form, &$form_state) {
  if (isset($form_state['values']['material_id'])) {
    _ddb_cover_upload_delete_material($form_state['values']['material_id']);

    // Clear cover and materials cache.
    cache_clear_all('ddb_cover_upload', 'cache', TRUE);
  }

  drupal_set_message(t('The material have been deleted. It might take sometime before it is delete in all systems.'));
  $form_state['redirect'] = 'admin/config/cover_upload/upload/list';
}

/**
 * Parse cover IIR to get cover Id.
 *
 * @param $ref
 *   The IIR ref.
 *
 * @return string
 */
function _ddb_cover_upload_parse_cover_id($ref) {
  $parts = explode('/', $ref);
  return array_pop($parts);
}

/**
 * Get materials from the service.
 *
 * @param int $page
 *   The page to use.
 *
 * @return \CoverServiceUpload\Model\MaterialRead[]
 * @throws \CoverServiceUpload\ApiException
 * @throws \GuzzleHttp\Exception\GuzzleException
 */
function _ddb_cover_upload_get_materials($page = 1) {
  $materials = &drupal_static(__FUNCTION__);
  if (!isset($materials)) {
    $cache = cache_get('ddb_cover_upload_materials');
    if ($cache && $cache->expire > REQUEST_TIME) {
      $materials = $cache->data;
    }
    else {
      $config = ddb_cover_upload_client_config();
      $apiInstance = new CoverServiceUpload\Api\MaterialApi(
        new GuzzleHttp\Client(),
        $config
      );

      $materials = $apiInstance->getMaterialCollection($page);

      // Expire after one day (will be cleared after uploading new covers).
      $expire = REQUEST_TIME + 86400;
      cache_set('ddb_cover_upload_materials', $materials, 'cache', $expire);
    }
  }

  return $materials;
}

/**
 * Get cover base on id.
 *
 * @param $id
 *   The id of the cover to get.
 *
 * @return \CoverServiceUpload\Model\CoverCoverRead
 *   The cover to fetch.
 *
 * @throws \CoverServiceUpload\ApiException
 * @throws \GuzzleHttp\Exception\GuzzleException
 */
function _ddb_cover_upload_get_cover($id) {
  // @TODO: cache
  $config = ddb_cover_upload_client_config();
  $apiInstance = new CoverServiceUpload\Api\CoverApi(
    new GuzzleHttp\Client(),
    $config
  );

  return $apiInstance->getCoverItem($id);
}

/**
 * Delete material.
 *
 * @param $id
 *   The id of the material to delete.
 *
 * @throws \CoverServiceUpload\ApiException
 * @throws \GuzzleHttp\Exception\GuzzleException
 */
function _ddb_cover_upload_delete_material($id) {
  $config = ddb_cover_upload_client_config();
  $apiInstance = new CoverServiceUpload\Api\MaterialApi(
    new GuzzleHttp\Client(),
    $config
  );

  $apiInstance->deleteMaterialItem($id);
}
