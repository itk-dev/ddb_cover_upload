<div>
  <h3>Material information</h3>
  <div class="content">
    <?php if(!empty($material)) : ?>
      <div class="field field-label-inline">
        <div class="field-label"><?php print t('Post id') . ':';?>&nbsp;</div>
        <div><?php print $material->getId(); ?></div>
      </div>
      <div class="field field-label-inline">
        <div class="field-label"><?php print t('Material name') . ':';?>&nbsp;</div>
        <div><?php print $material->getTitle(); ?></div>
      </div>
    <?php else : ?>
      <span class="changed-warning">(<?php print t('Missing material'); ?>)</span>
    <?php endif; ?>
  </div>
  <h3>Image metadata
    <?php if ($meta_data['image_changed']) : ?>
      <span class="changed-warning">(<?php print t('Changed'); ?>)</span>
    <?php endif; ?>
  </h3>
  <div class="content">
    <div class="field field-label-inline">
      <div class="field-label"><?php print t('Image name') . ':';?>&nbsp;</div>
      <div>
        <?php print $meta_data['file_name'];?>
      </div>
    </div>
    <div class="field field-label-inline">
      <div class="field-label"><?php print t('Image dimensions') . ':';?>&nbsp;</div>
      <div><?php print $meta_data['file_dimensions']['x'] . 'X' . $meta_data['file_dimensions']['y'];?></div>
    </div>
    <div class="field field-label-inline">
      <div class="field-label"><?php print t('Image file type') . ':';?>&nbsp;</div>
      <div><?php print $meta_data['file_type'];?></div>
    </div>
    <div class="field field-label-inline">
      <div class="field-label"><?php print t('Image file size') . ':';?>&nbsp;</div>
      <div><?php print $meta_data['file_size'] / 1000;?>KB</div>
    </div>
    <div class="field field-label-inline">
      <div class="field-label"><?php print t('Author') . ':';?>&nbsp;</div>
      <div><?php print $meta_data['author']?></div>
    </div>
  </div>
</div>
