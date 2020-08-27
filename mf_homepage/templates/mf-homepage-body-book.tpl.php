<?php

/**
 * @file
 * MF Homepage Lear it, Earn it, Bearn it block template.
 */
?>

<?php if (!empty($items)) : ?>
<div class="page-wrapper">
  <section class="body-book cf">

    <header class="body-book__header cf">
    <strong class="body-book__title"><?php print $block_title; ?></strong>
    <?php if (isset($sponsored_logo) && !empty($sponsored_logo) && isset($sponsored_logo['#link'])) :?>
      <?php if (!empty($sponsored_logo['#images']['desktop'])): ?>
      <a href="<?php print $sponsored_logo['#link']; ?>" target="_blank" class="body-book__sponsored-by desktop">
        <?php print $sponsored_logo['#images']['desktop']; ?>
      </a>
      <?php endif; ?>
      <?php if (!empty($sponsored_logo['#images']['mobile'])): ?>
      <a href="<?php print $sponsored_logo['#link']; ?>" target="_blank" class="body-book__sponsored-by mobile">
        <?php print $sponsored_logo['#images']['mobile']; ?>
      </a>
      <?php endif; ?>
    <?php endif; ?>
    </header>

    <div class="wrapper-srollable">
      <div class="body-book__inner">
        <?php foreach ($items as $item) : ?>
        <div class="body-book__item">
        <div class="media-block__body-mobile">
          <strong class="media-block__headline">
          <?php print $item['lead']['title']; ?>
          <svg class="icon svg-icon_arrow_up-dims arrow-down">
            <use xmlns:xlink="http://www.w3.org/1999/xlink" xlink:href="#icon_arrow_up"></use>
          </svg>
          </strong>
        </div>

        <div class="body-book__item-inner">
          <div class="media-block media-block_tile">
          <a href="<?php print $item['lead']['path']; ?>">
            <div class="media-block__media">
            <?php if ($item['lead']['is_video']): ?>
              <svg class="icon svg-icon_play_80-dims">
              <use xmlns:xlink="http://www.w3.org/1999/xlink" xlink:href="#icon_play_80"></use>
              </svg>
            <?php endif; ?>
            <?php if ($item['lead']['is_gallery']): ?>
              <svg class="icon svg-icon_gallery_80-dims">
              <use xmlns:xlink="http://www.w3.org/1999/xlink" xlink:href="#icon_gallery_80"></use>
              </svg>
            <?php endif; ?>
            <?php print $item['lead']['image']; ?>
            </div>

            <div class="media-block__body">
            <strong class="media-block__headline">
              <?php print $item['lead']['title']; ?>
            </strong>
            </div>
          </a>
          </div>
          <?php if (isset($item['list'])) : ?>
          <ul class="media-list">
            <?php foreach ($item['list'] as $list_item) : ?>
            <li class="media-list__item">
              <div class="media-block">
              <div class="media-block__media">
                <a href="<?php print $list_item['path']; ?>">
                <?php print $list_item['image']; ?>
                </a>
              </div>
              <div class="media-block__body">
                <strong class="media-block__headline">
                <?php print $list_item['title']; ?>
                </strong>
              </div>
              </div>
            </li>
            <?php endforeach; ?>
          </ul>
          <?php endif; ?>
        </div>
        </div>
        <?php endforeach; ?>
      </div>
    </div>

  </section>
</div>
<?php endif; ?>
