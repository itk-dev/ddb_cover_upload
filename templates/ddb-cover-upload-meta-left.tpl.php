<?php
/**
 * @file
 * Template for showing image and upload dialog.
 *
 * Available variables:
 * - $original_image: The original image themed.
 * - $displayed_image: URL of the displayed image.
 * - $altered_time: A timestamp of last alteration.
 * - $adapt_link_title: Link label for opening dialog.
 */
?>
<div class="overflow-hidden">
  <p>
    <a href="#" class="btn btn-primary opener-modal" data-dialog="dialog-image"><?php print $variables['adapt_link_title']; ?></a>
  </p>
  <img class="img-displayed" src="<?php print $variables['displayed_image']; ?>?m=<?php print $variables['altered_time']; ?>">
  <?php print $variables['original_image']; ?>
</div>
<div tabindex="-1" role="dialog-image" style="display: none;">
  <div id="dialog-image" class="ui-dialog-content ui-widget-content" data-title="<?php print t('Adapt image'); ?>">
    <div id="crop-upload">
      <div class="toolbar">
        <a href="#" class="crop-rotate btn btn-primary rotation" data-degrees="90"><?php print t('Rotate') . ' 90'; ?></a>
        <a href="#" class="crop-rotate btn btn-primary rotation" data-degrees="-90"><?php print t('Rotate') . ' -90'; ?></a>
        <a href="#" class="crop-rotate btn btn-primary rotation" data-degrees="10"><?php print t('Rotate') . ' 10'; ?></a>
        <a href="#" class="crop-rotate btn btn-primary rotation" data-degrees="-10"><?php print t('Rotate') . ' -10'; ?></a>
        <a href="#" class="crop-rotate btn btn-primary rotation" data-degrees="1"><?php print t('Rotate') . ' 1'; ?></a>
        <a href="#" class="crop-rotate btn btn-primary rotation" data-degrees="-1"><?php print t('Rotate') . ' -1'; ?></a>
        <a href="#" class="crop-rotate btn btn-primary mirror-x"><?php print t('Mirror X'); ?></a>
        <a href="#" class="crop-rotate btn btn-primary mirror-y"><?php print t('Mirror Y'); ?></a>
        <a href="#" class="btn btn-primary brightness" id="brightness"><?php print t('Increase brightness'); ?></a>
        <a href="#" class="btn btn-primary brightness" id="brightness-neg"><?php print t('Reduce brightness'); ?></a>
        <a href="#" class="btn btn-primary contrast" id="contrast"><?php print t('Increase contrast'); ?></a>
        <a href="#" class="btn btn-primary contrast" id="contrast-neg"><?php print t('Reduce contrast'); ?></a>
        <a href="#" class="btn btn-primary image-reset"><?php print t('Reset image'); ?></a>
      </div>
      <div style="max-width:800px;margin-top: 1em;">
        <canvas id="canvas" class="img-responsive"></canvas>
      </div>
      <p>
        <a href="#" id="img-submit" class="save-alt btn btn-primary"><?php print t('Save changes');?></a>
      </p>
    </div>
  </div>
</div>
