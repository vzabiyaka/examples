<?php

/**
 * @file
 * MFRenderHomepageBlocks class file.
 */

/**
 * Class to process and display nodequeue based blocks.
 *
 * Class MFRenderHomepageBlocks.
 */
class MFRenderHomepageBlocks extends MFConfigBlocks {

  /**
   * Helper to get homepage Top 3 block.
   *
   * @param string $delta
   *   Block delta.
   *
   * @return array
   *   Render array of video blocks markup.
   */
  public static function getHomepageTop3Block($delta) {
    $nodes = self::getBlockNodes($delta);
    if (!empty($nodes)) {
      // Get first node.
      reset($nodes);
      if (is_array($nodes)) {
        $first_key = key($nodes);
        $first_node = $nodes[$first_key];
        unset($nodes[$first_key]);
      }
      elseif (is_object($nodes)) {
        $first_node = $nodes;
      }

      // If we have more then one node to display.
      if (!empty($nodes)) {
        $bottom_nodes = node_view_multiple($nodes, 'teaser');
        if (!empty($bottom_nodes)) {
          $bottom_nodes = reset($bottom_nodes);
          unset($bottom_nodes['#sorted']);
        }
      }
      $sharethrough_en = '';
      if (variable_get('ami_sharethrough_en', FALSE) && $delta == 'mf_homepage_top3') {
        if (count($bottom_nodes) < 2) {
          $bottom_nodes['sharethrough'] = '';
          $bottom_nodes['sharethrough_mob'] = '';
        }
        else {
          end($bottom_nodes);
          $last_node_key = key($bottom_nodes);
          $last_node = $bottom_nodes[$last_node_key];
          unset($bottom_nodes[$last_node_key]);
          $bottom_nodes['sharethrough'] = '';
          $bottom_nodes['sharethrough_mob'] = '';
          $bottom_nodes[$last_node_key] = $last_node;
          $sharethrough_en = $last_node_key;
        }
      }
      // Add Distroscale.
      if (module_exists('ami_distroscale')) {
        $distroscale_en = ($delta == 'mf_homepage_top3') ? ami_distroscale_status(TRUE) : FALSE;
        if ($distroscale_en && $delta == 'mf_homepage_top3') {
          $markup = self::renderBlock('ami_distroscale', 'ami_distroscale_block');
          $bottom_nodes = array(
              'distroscale' => array(
                '#markup' => $markup,
              ),
            ) + (array) $bottom_nodes;
        }
      }

      // Use mf_homepage_top3 as custom theme for block content.
      return array(
        '#theme' => 'mf_homepage_top3',
        '#top' => node_view($first_node, 'big_teaser'),
        '#bottom' => !empty($bottom_nodes) ? $bottom_nodes : '',
        '#class' => 'homepage',
        '#sharethrough_en' => $sharethrough_en,
      );
    }
  }

  /**
   * Helper to get homepage videos block.
   *
   * @param string $delta
   *   Block delta.
   *
   * @return array
   *   Render array of video blocks markup.
   */
  public static function getHomepageVideosBlock($delta) {
    $nodes = self::getBlockNodes($delta);
    if (!empty($nodes)) {
      // Get all block nodes.
      // Get first node.
      reset($nodes);
      $first_key = key($nodes);
      $first_node = node_view($nodes[$first_key], 'big_teaser');
      unset($nodes[$first_key]);
      // If we have more then one node then process them as regular teasers.
      if (!empty($nodes)) {
        $nodes = node_view_multiple($nodes);
        if (!empty($nodes) && is_array($nodes)) {
          // Change first node wide image image style.
          if (!empty($first_node['field_promo_image'])) {
            $first_node['field_promo_image'][0]['#image_style'] = 'wide_videos';
          }
          // Change image style for small videos.
          foreach ($nodes['nodes'] as $key => $value) {
            if (!empty($value['field_promo_image'])) {
              $nodes['nodes'][$key]['field_promo_image'][0]['#image_style'] = 'small_videos';
            }
          }
          // Remove sorted key from array.
          unset($nodes['#sorted']);
          // Use mf_homepage_videos as custom theme for block content.
          return array(
            '#theme' => 'mf_homepage_videos',
            '#big_video' => $first_node,
            '#small_videos' => !empty($nodes) ? $nodes : '',
          );
        }
      }
    }
  }

  /**
   * Helper to get homepage videos block.
   *
   * @param string $delta
   *   Block delta.
   *
   * @return array
   *   Render array of video blocks markup.
   */
  public static function getHomepageTodaysWorkoutBlock($delta) {
    $nodes = self::getBlockNodes($delta);
    if (!empty($nodes) && is_array($nodes)) {
      $node = reset($nodes);
      if (!empty($node->nid)) {
        $image = self::getPromoImage($node, '390x260');
        $promo_title = self::getPromoTitle($node);
        // Use custom theme mf_homepage_todays_workout to render block.
        return array(
          '#theme' => 'mf_homepage_todays_workout',
          '#date' => date('n.d', time()),
          '#image' => !empty($image) ? $image : '',
          '#title' => !empty($promo_title) ? $promo_title : '',
          '#node_path' => drupal_get_path_alias('node/' . $node->nid),
        );
      }
    }
  }

  /**
   * Helper to get homepage videos block.
   *
   * @param string $delta
   *   Block delta.
   *
   * @return array
   *   Render array of video blocks markup.
   */
  public static function getHomepageEditorsPicksBlock($delta) {
    $nodes = self::getBlockNodes($delta);
    if (!empty($nodes) && is_array($nodes)) {
      $variables = array();
      foreach ($nodes as $node) {
        $image = self::getPromoImage($node, 'wide_node_teaser');
        $promo_title = self::getPromoTitle($node);
        $topics = self::getTopics($node, NULL, 'text');
        $author = self::getAuthor($node, NULL, 'text');
        $class = MFNodeBlocks::getChannelClass($node);

        $variables[] = array(
          'image' => !empty($image) ? $image : '',
          'title' => !empty($promo_title) ? $promo_title : '',
          'topics' => !empty($topics) ? $topics : '',
          'path' => drupal_get_path_alias('node/' . $node->nid),
          'class' => !empty($class) ? $class : '',
          'author' => !empty($author) ? $author : '',
          'is_video' => $node->type == 'video' ? TRUE : FALSE,
          'is_gallery' => $node->type == 'photo_gallery' ? TRUE : FALSE,
          'todays' => self::checkTodaysNode($node),
          'todays_news' => MFLoadEntities::checkTodaysNews($node->nid),
        );
      }
      // Use custom theme mf_homepage_editors_pick to render blocks.
      return array(
        '#theme' => 'mf_homepage_editors_pick',
        '#items' => $variables,
      );
    }
  }

  /**
   * Helper to get homepage videos block.
   *
   * @param string $delta
   *   Block delta.
   *
   * @return array
   *   Render array of video blocks markup.
   */
  public static function getHomepageLeftNewsBlock($delta) {
    $nodes = self::getBlockNodes($delta);
    if (!empty($nodes) && is_array($nodes)) {
      $variables = array();

      foreach ($nodes as $node) {
        $promo_title = self::getPromoTitle($node);
        $image = self::getPromoImage($node, 'news_block_image');
        $topics = self::getTopics($node, array(), 'text');
        $class = MFNodeBlocks::getChannelClass($node);
        $variables[] = array(
          'title' => !empty($promo_title) ? $promo_title : '',
          'image' => !empty($image) ? $image : '',
          'path' => drupal_get_path_alias('node/' . $node->nid),
          'topics' => !empty($topics) ? $topics : '',
          'class' => !empty($class) ? $class : '',
        );
      }
      // Use custom theme mf_homepage_left_news to render block.
      return array(
        '#theme' => 'mf_homepage_left_news',
        '#items' => $variables,
        '#class' => 'homepage',
      );
    }
  }

  /**
   * Top group block.
   *
   * Helper to collect all top group blocks into one big block.
   *
   * @return array
   *   Return rendered array of top group block.
   */
  public static function getHomepageBlocksGroup($blocks) {
    $content = array();
    foreach ($blocks as $position => $block) {
      foreach ($block as $delta => $module) {
        $content[$position][$delta] = self::renderBlock($module, $delta);
      }
    }

    return array(
      '#theme' => 'mf_homepage_group',
      '#content' => $content,
    );
  }

  /**
   * Get left rail homepage average teasers lists.
   *
   * @param string $delta
   *   Block delta.
   *
   * @return array
   *   Render array of block output.
   */
  public static function getHomepageAverageTeasers($delta) {
    $nodes = self::getBlockNodes($delta);
    if (!empty($nodes)) {
      $nodes = node_view_multiple($nodes, 'average_teaser');
      $nodes = reset($nodes);
      unset($nodes['#sorted']);
      $count = count($nodes);
      if ($count > 2) {
        $index = 0;
        foreach ($nodes as $key => $node) {
          if ($index != 0) {
            // Hide image if more then two items.
            unset($nodes[$key]['field_promo_image']);
          }
          $index++;
        }
      }

      return $nodes;
    }
  }

  /**
   * Helper to get Homepage LEB block content.
   *
   * Learn it, Earn it, Burn it.
   *
   * @param array $delta
   *   Settings array.
   *
   * @return array
   *   Homepage Learn it, Earn it, burn it block.
   */
  public static function getHomepageLebBlock($delta) {
    $nodes = self::getBlockNodes($delta);
    $settings = variable_get($delta);
    if (!empty($nodes)) {
      $items = array();
      $attributes = array(
        'class' => 'block-media__img',
      );
      foreach ($nodes as $key => $node) {
        $items[$key]['flag'] = MFLoadEntities::getEntityFieldValue('node', $node, 'field_flag');
        $items[$key]['title'] = l(MFBlocks::getPromoTitle($node), 'node/' . $node->nid);
        $items[$key]['byline'] = MFLoadEntities::getEntityFieldValue('node', $node, 'field_byline');;
        $items[$key]['path'] = drupal_get_path_alias('node/' . $node->nid);
        $items[$key]['image'] = MFBlocks::getPromoImage($node, 'wide_node_teaser', $attributes);
        $items[$key]['description'] = MFBlocks::getPromoDescription($node);
      }
      $sponsored_logo = '';
      if (isset($settings['sponsored_logo']) && !empty($settings['sponsored_logo'])) {
        $sponsored_logo = MFNodeBlocks::getSponsoredByBlock((int) $settings['sponsored_logo']);
      }

      return array(
        '#theme' => 'mf_homepage_leb',
        '#items' => $items,
        '#sponsored_logo' => $sponsored_logo,
      );
    }
  }

  /**
   * Helper to get Homepage Body book block content.
   *
   * @param array $delta
   *   Settings array.
   *
   * @return array
   *   Homepage Learn it, Earn it, burn it block.
   */
  public static function getHomepageBodyBookBlock($delta) {
    $nodes = self::getBlockNodes($delta);
    $settings = variable_get($delta);
    if (!empty($nodes)) {
      $block_title = db_query('SELECT title FROM {block} WHERE module = :module AND delta = :delta AND theme = :theme', array(
        ':module' => 'mf_homepage',
        ':delta' => 'mf_homepage_body_book_block',
        ':theme' => 'mfxvi'
      ))->fetchField();
      $block_title = ($block_title == '<none>') ? '' : $block_title;
      $items = array();
      $attributes = array(
        'class' => 'block-media__img',
        'default_fallback_image' => TRUE,
      );
      $i = 1;
      $k = 1;
      foreach ($nodes as $key => $node) {
        if ($i < 4) {
          $image_style = 'wide_node_teaser';
          $items[$i]['lead']['title'] = l(MFBlocks::getPromoTitle($node), 'node/' . $node->nid);
          $items[$i]['lead']['path'] = drupal_get_path_alias('node/' . $node->nid);
          $items[$i]['lead']['image'] = MFBlocks::getPromoImage($node, $image_style, $attributes);
          $items[$i]['lead']['is_video'] = $node->type == 'video' ? TRUE : FALSE;
          $items[$i]['lead']['is_gallery'] = $node->type == 'photo_gallery' ? TRUE : FALSE;
        }
        else {
          if ($k > 3) {
            $k = 1;
          }
          $image_style = 'small_node_teaser';
          $items[$k]['list'][$key]['title'] = l(MFBlocks::getPromoTitle($node), 'node/' . $node->nid);
          $items[$k]['list'][$key]['path'] = drupal_get_path_alias('node/' . $node->nid);
          $items[$k]['list'][$key]['image'] = MFBlocks::getPromoImage($node, $image_style, $attributes);
          $k++;
        }
        $i++;
      }
      $sponsored_logo = '';
      if (isset($settings['sponsored_logo']) && !empty($settings['sponsored_logo'])) {
        $image_classes = 'icon';
        $sponsored_logo = MFNodeBlocks::getSponsoredByBlock((int) $settings['sponsored_logo'], $image_classes);
      }

      return array(
        '#theme' => 'mf_homepage_body_book',
        '#items' => $items,
        '#sponsored_logo' => $sponsored_logo,
        '#block_title' => $block_title,
      );
    }
  }

  /**
   * Helper to get Homepage Promo Banner block content.
   *
   * @param array $delta
   *   Settings array.
   *
   * @return array
   *   Homepage Learn it, Earn it, burn it block.
   */
  public static function getHomepagePromoBannersBlock($delta) {
    $settings = variable_get('mf_homepage_promo_banners');
    if ($settings) {
      $delta = ($delta == 'mf_homepage_promo_banners_1') ? 'first_block' : 'second_block';
      $vars = $settings[$delta];
      if (!empty($vars['desktop']) || !empty($vars['mobile'])) {
        return array(
          '#theme' => 'mf_homepage_promo_banners',
          '#desktop' => $vars['desktop'],
          '#mobile' => $vars['mobile'],
        );
      }
    }
  }

}
